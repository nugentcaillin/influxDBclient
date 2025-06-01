#include "influxdbclient/client/InfluxDBClient.hpp"
#include "influxdbclient/networking/i_http_client.hpp"
#include "influxdbclient/networking/task.hpp"
#include "influxdbclient/networking/http_request.hpp"
#include "influxdbclient/networking/http_response.hpp"
#include <nlohmann/json.hpp>
#include <coroutine>
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
	
	influxdbclient::networking::Task<int> health = getHealth();
	health._handle.resume();
	auto health_future = health.get();
	health_future.wait();
	int status = health_future.get();
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

	
	std::cout << "making health req" << std::endl;

	//influxdbclient::networking::Task<influxdbclient::networking::>

	influxdbclient::networking::HttpResponse res = co_await _httpClient->performAsync(req);

	std::cout << "finished health req" << std::endl;
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





}

}


