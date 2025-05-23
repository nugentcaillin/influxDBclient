#ifndef INFLUX_DB_H
#define INFLUX_DB_H



#include <vector>
#include <map>
#include <string>
#include <spdlog/spdlog.h>
#include <iostream>
#include <spdlog/sinks/null_sink.h>

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
	);


	/**
	 * @brief constructs an InfluxDBClient using a no op logger and specified batch size
	 *
	 * @param url the InfluxDB url (e.g. http://localhost:8086)
	 * @param org the InfluxDB organisation name 
	 * @param token the InfluxDB API token
	 * @param batch_size the number of data points to buffer before writing to the database
	 */
	InfluxDBClient
	( const std::string& url
	, const std::string& org
	, const std::string& token
	, int batch_size
	);

	/**
	 * @brief constructs an InfluxDBClient using a specified logger and default	 
	 * batch size of 5000
	 * @param url the InfluxDB url (e.g. http://localhost:8086)
	 * @param org the InfluxDB organisation name 
	 * @param token the InfluxDB API token
	 * @param logger a shared pointer to the spd logger to use for logging
	 */
	InfluxDBClient
	( const std::string& url
	, const std::string& org
	, const std::string& token
	, std::shared_ptr<spdlog::logger> logger
	);
	
	/**
	 * @brief constructs an InfluxDBClient using a no op logger and default
	 * batch size of 5000
	 * @param url the InfluxDB url (e.g. http://localhost:8086)
	 * @param org the InfluxDB organisation name 
	 * @param token the InfluxDB API token
	 */
	InfluxDBClient
	( const std::string& url
	, const std::string& org
	, const std::string& token
	);

	
};

}
}

#endif
