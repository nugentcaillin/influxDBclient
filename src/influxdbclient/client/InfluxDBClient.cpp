#include "influxdbclient/client/InfluxDBClient.hpp"
#include "influxdbclient/networking/i_http_client.hpp"
#include "influxdbclient/networking/task.hpp"
#include "influxdbclient/networking/http_request.hpp"
#include "influxdbclient/networking/http_response.hpp"
#include "influxdbclient/data/write_buffer.hpp"
#include "influxdbclient/data/measurement.hpp"
#include <nlohmann/json.hpp>
#include <map>
#include <coroutine>
#include <vector>
#include <stdexcept>
#include <iostream>
namespace influxdbclient
{
namespace client
{

#define DEFAULT_LOGGER spdlog::null_logger_mt("influx_db_client_global_logger")

std::shared_ptr<spdlog::logger>
getOrCreateGlobalLogger()
{
	auto logger = spdlog::get("influx_db_client_global_logger");
	if (logger.get() == nullptr)
	{
		logger = DEFAULT_LOGGER;
	}
	return logger;
}

influxdbclient::networking::HttpRequest
InfluxDBClient::createBasicRequest()
{
	influxdbclient::networking::HttpRequest req;
	
	req.setUrl(_url);

	std::string auth = "Bearer ";
	auth += _token;
	req.addHeader("Authorization", auth);
	return req;
}


InfluxDBClient::InfluxDBClient
( const std::string& url
, const std::string& org
, const std::string& token
, int batch_size
, const std::shared_ptr<spdlog::logger> logger
, const std::shared_ptr<influxdbclient::networking::IHttpClient> httpClient)
: _url(url)
, _token(token)
, _org(org)
, _batch_size(batch_size)
, _logger(std::move(logger))
, _httpClient(httpClient)
{
	// defensive check for logger, warn user and initialise as global or null sink logger
	if (_logger.get() == nullptr)
	{
		std::cerr << "WARNING: InfluxDB Client recieved null logger pointer, falling back to global logger or null logger\n";
		_logger = getOrCreateGlobalLogger();
	}
	
	// check for invalid bucket size
	if (_batch_size < 1)
	{
		_logger->warn("Batch size of {} is invalid. changing to default of 5000", _batch_size);
		_batch_size = 5000;
	}

	
	// blocking until health check OK
	auto health = getHealth();
	int status = health.get();
	
	if (status != 200)
	{
		std::string err = "Health check failed with status: ";
		err += std::to_string(status);
		_logger->error(err);
		throw std::runtime_error(err);
	}
	_logger->info("Health check OK");
	_logger->info("Influx db client initialised with batch size of {}", _batch_size);
	
}

influxdbclient::networking::Task<int>
InfluxDBClient::getHealth
()
{
	influxdbclient::networking::HttpRequest req = createBasicRequest();
	req.appendToUrl("/health");
	req.setMethod(influxdbclient::networking::HttpMethod::GET);

	

	//influxdbclient::networking::Task<influxdbclient::networking::>

	auto t = _httpClient->performAsync(req);
	influxdbclient::networking::HttpResponse res = co_await t;
	std::cout << "health request performed" << std::endl;
	co_return res.http_status;
}


influxdbclient::networking::Task<std::vector<std::string>> 
InfluxDBClient::listDatabases
()
{
	influxdbclient::networking::HttpRequest req = createBasicRequest();
	req.setMethod(influxdbclient::networking::HttpMethod::GET);
	req.appendToUrl("/api/v3/configure/database?format=json");

	
	influxdbclient::networking::HttpResponse res = co_await _httpClient->performAsync(req);
	
	if (res.http_status == 400) {
		throw std::runtime_error("Bad request");
	}
	if (res.http_status == 401) {
		throw std::runtime_error("Unauthorized access");
	}
	if (res.http_status == 404) {
		throw std::runtime_error("Database not found");
	}
	_logger->info("list database");

	nlohmann::json data = nlohmann::json::parse(res.body);	
	std::vector<std::string> ret;

	// get string names out of json object
	if (data.is_array()) {
		for (auto& el: data.items()) {
			if (el.value().contains("iox::database")) ret.push_back(*(el.value().find("iox::database")));
		}
	}
	

	co_return ret;
}


influxdbclient::networking::Task<void> 
InfluxDBClient::createDatabase
( std::string name)
{
	influxdbclient::networking::HttpRequest req = createBasicRequest();
	req.setMethod(influxdbclient::networking::HttpMethod::POST);
	req.appendToUrl("/api/v3/configure/database");
	req.addHeader("Content-Type", "application/json");

	nlohmann::json data;
	data.emplace("db", name);

	req.setBody(data.dump());


	influxdbclient::networking::HttpResponse res = co_await _httpClient->performAsync(req);

	
	if (res.http_status == 400 ) {
		throw std::runtime_error("Bad request");
	}
	if (res.http_status == 401) {
		throw std::runtime_error("Unauthorized access");
	}
	if (res.http_status == 409) {
		throw std::runtime_error("Database already exists");
	}

	_logger->info("created database {}", name);


	co_return;
}


influxdbclient::networking::Task<void> 
InfluxDBClient::writeMeasurement
( const influxdbclient::data::Measurement& measurement
, const std::string& name
, influxdbclient::data::TimePrecision precision)
{
	std::cout << "entering write" << std::endl;
	// find buffer or create one
	std::cout << "finding write buffer" << std::endl;
	auto it = _writeBuffers.find({precision, name});
	if (it == _writeBuffers.end())
	{
		std::cout << "creatinng write buffer, not found" << std::endl;
		it = _writeBuffers.emplace(std::pair<influxdbclient::data::TimePrecision, std::string>{precision, name}, influxdbclient::data::WriteBuffer(name, precision, _batch_size)).first;
	}
	std::cout << "adding measurement" << std::endl;
	std::cout << "buffer name" << it->second.getName();

	// add measurement and flush if needed 
	it->second.addMeasurement(measurement);
	if (it->second.isFull())
	{
		std::cout << "flushing" << std::endl;
		co_await flushWriteBuffer(name, precision);
	}
	co_return;
}
influxdbclient::networking::Task<void> 
InfluxDBClient::flushWriteBuffer
( const std::string& name
, influxdbclient::data::TimePrecision precision)
{
	std::cout << "made it this far" << std::endl;
	auto it = _writeBuffers.find({precision, name});

	auto drained = it->second.drainMeasurements();	

	// create and await POST request here 
	
	std::string precisionStr;
	switch(precision)
	{
		case influxdbclient::data::TimePrecision::MILLISECONDS:
			precisionStr = "millisecond";
			break;
		case influxdbclient::data::TimePrecision::SECONDS:
			precisionStr = "second";
			break;
		case influxdbclient::data::TimePrecision::MICROSECONDS:
			precisionStr = "microsecond";
			break;
		case influxdbclient::data::TimePrecision::NANOSECONDS:
			precisionStr = "nanosecond";
			break;
		default:
			precisionStr = "auto";
	}


	if (it == _writeBuffers.end()) 
	{
		_logger->warn("write of empty buffer attempted, name: {}, precision: {}", name, precisionStr);
		co_return;
	}

	
	influxdbclient::networking::HttpRequest req = createBasicRequest();
	req.setMethod(influxdbclient::networking::HttpMethod::POST);
	req.appendToUrl("/api/v3/write_lp");
	req.appendToUrl("?db=" + name);
	req.appendToUrl("&precision=" + precisionStr);
	req.addHeader("Accept", "application/json");
	req.addHeader("Content-Encoding", "identity");
	req.addHeader("Content-Type", "text/plain; charset=utf-8");
	req.addHeader("Content-Length", std::to_string(drained.first.length()));
	
	req.setBody(std::move(drained.first));
	
	
	influxdbclient::networking::HttpResponse res = co_await _httpClient->performAsync(req);
	
	
	if (res.http_status == 400) {
		throw std::runtime_error("Bad request");
	}
	if (res.http_status == 401) {
		throw std::runtime_error("Unauthorized access");
	}
	if (res.http_status == 403) {
		throw std::runtime_error("Access denied");
	}
	if (res.http_status == 413) {
		throw std::runtime_error("Request entity too large");
	}
	if (res.http_status == 422) {
		throw std::runtime_error("Unprocessable entity");
	}
	if (res.http_status != 204) {
		throw std::runtime_error("Unexpected error, status: " + std::to_string(res.http_status));
	}

	_logger->info("Written {} measurements to {} with precision: {}", drained.second, name, precisionStr);
	
	co_return;
}


}

}


