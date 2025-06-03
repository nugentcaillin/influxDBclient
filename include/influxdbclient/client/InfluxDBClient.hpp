#ifndef INFLUX_DB_H
#define INFLUX_DB_H



#include <vector>
#include <map>
#include <string>
#include <spdlog/spdlog.h>
#include <iostream>
#include <spdlog/sinks/null_sink.h>
#include "influxdbclient/networking/i_http_client.hpp"
#include "influxdbclient/networking/task.hpp"
#include "influxdbclient/networking/http_request.hpp"
#include "influxdbclient/data/measurement.hpp"
#include "influxdbclient/data/write_buffer.hpp"
#include <coroutine>
#include <memory>
#include <map>
#include <vector>
#include <utility>
#include <nlohmann/json.hpp>

namespace influxdbclient
{


namespace client
{

class InfluxDBClient 
{
private:
	std::string _url;
	std::string _org;
	std::string _token;
	int _batch_size;
	std::shared_ptr<spdlog::logger> _logger;
	std::shared_ptr<influxdbclient::networking::IHttpClient> _httpClient;
	std::map<std::pair<influxdbclient::data::TimePrecision, std::string>, influxdbclient::data::WriteBuffer> _writeBuffers;
	
	influxdbclient::networking::HttpRequest createBasicRequest();

public:
	/**
	 * @brief constructs an InfluxDBClient using a specified logger and batch size
	 * @param url the InfluxDB url (e.g. http://localhost:8086)
	 * @param org the InfluxDB organisation name 
	 * @param token the InfluxDB API token
	 * @param logger a shared pointer to the spd logger to use for logging
	 * @param batch_size the number of data points to buffer before writing to the database
	 */
	InfluxDBClient
	( const std::string& url
	, const std::string& org
	, const std::string& token
	, int batch_size
	, std::shared_ptr<spdlog::logger> logger
	, const std::shared_ptr<influxdbclient::networking::IHttpClient> httpClient);

	
	// convenience constructors

	// no batch size
	InfluxDBClient
	( const std::string& url
	, const std::string& org
	, const std::string& token
	, std::shared_ptr<spdlog::logger> logger
	, const std::shared_ptr<influxdbclient::networking::IHttpClient> httpClient);
	

	// no logger
	InfluxDBClient
	( const std::string& url
	, const std::string& org
	, const std::string& token
	, int batch_size
	, const std::shared_ptr<influxdbclient::networking::IHttpClient> httpClient);
	

	// no httpClient
	InfluxDBClient
	( const std::string& url
	, const std::string& org
	, const std::string& token
	, int batch_size
	, std::shared_ptr<spdlog::logger> logger);	

	
	// no batch size, logger
	InfluxDBClient
	( const std::string& url
	, const std::string& org
	, const std::string& token
	, const std::shared_ptr<influxdbclient::networking::IHttpClient> httpClient);
	
	// no batch size, httpclient
	InfluxDBClient
	( const std::string& url
	, const std::string& org
	, const std::string& token
	, std::shared_ptr<spdlog::logger> logger);
	
	// no logger, httpclient
	InfluxDBClient
	( const std::string& url
	, const std::string& org
	, const std::string& token
	, int batch_size);
	
	// no logger, batch size, http client
	InfluxDBClient
	( const std::string& url
	, const std::string& org
	, const std::string& token);
	



	// cache data 
	
	// database
	
	influxdbclient::networking::Task<std::vector<std::string>> listDatabases();
	influxdbclient::networking::Task<void> createDatabase(std::string name);
	
	// processing engine
	
	// server information
	influxdbclient::networking::Task<int> getHealth();

	// table
	
	// token
	
	// query data
	influxdbclient::networking::Task<nlohmann::json>
	querySql
	( const std::string& database
	, const std::string& query
	, const std::map<std::string, std::string> params);
	
	// write data 
	
	influxdbclient::networking::Task<void> 
	writeMeasurement
	( const influxdbclient::data::Measurement& measurement
	, const std::string& name 
	, influxdbclient::data::TimePrecision precision);

	influxdbclient::networking::Task<void>
	flushWriteBuffer
	( const std::string& name
	, influxdbclient::data::TimePrecision precision);
	
};

}
}

#endif
