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
	std::string _token;
	std::string _org;
	int _batch_size;
	std::shared_ptr<spdlog::logger> _logger;

public:
	
	// user provided splogger
	InfluxDBClient
	( const std::string& url
	, const std::string& token
	, const std::string& org
	, int batch_size
	, std::shared_ptr<spdlog::logger> logger
	);


	// no provided logger
	InfluxDBClient
	( const std::string& url
	, const std::string& token
	, const std::string& org
	, int batch_size
	);
	
	
};

}
}

#endif
