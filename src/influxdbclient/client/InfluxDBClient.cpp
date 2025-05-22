#include "influxdbclient/client/InfluxDBClient.hpp"
namespace influxdbclient
{
namespace client
{

InfluxDBClient::InfluxDBClient
( const std::string& url
, const std::string& token
, const std::string& org
, int batch_size
, std::shared_ptr<spdlog::logger> logger
)
: _url(url)
, _token(token)
, _org(org)
, _batch_size(batch_size)
, _logger(std::move(logger))
{
	// defensive check for logger, warn user and initialise null sink logger
	if (_logger.get() == nullptr)
	{
		std::cerr << "WARNING: InfluxDB Client recieved null logger pointer, falling back to spdlog::null_logger_mt";
		_logger = spdlog::null_logger_mt("fallback_null_logger");
	}
	_logger->info("Influx db client initialised");
}


// no provided logger
InfluxDBClient::InfluxDBClient
( const std::string& url
, const std::string& token
, const std::string& org
, int batch_size
)
: InfluxDBClient(url, token, org, batch_size, spdlog::null_logger_mt("null_logger"))
{}


}

}


