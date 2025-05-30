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
#include <coroutine>
#include <memory>

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

public:
	/**
	 * @brief constructs an InfluxDBClient using a specified logger and batch size
	 *
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
	
	influxdbclient::networking::Task<int> getHealth();
	
};

}
}

#endif
