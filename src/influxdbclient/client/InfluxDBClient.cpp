#include "influxdbclient/client/InfluxDBClient.hpp"
namespace influxdbclient
{
namespace client
{

InfluxDBClient::InfluxDBClient
( const std::string& url
, const std::string& org
, const std::string& token
, int batch_size
, std::shared_ptr<spdlog::logger> logger
)
: _url(url)
, _token(token)
, _org(org)
, _batch_size(batch_size)
, _logger(std::move(logger))
{
	// defensive check for logger, warn user and initialise as global or null sink logger
	if (_logger.get() == nullptr)
	{
		std::cerr << "WARNING: InfluxDB Client recieved null logger pointer, falling back to global logger or null logger";
		_logger = spdlog::get("influx_db_client_global_logger");
	}
	if (_logger.get() == nullptr)
	{
		_logger = spdlog::null_logger_mt("influx_db_client_global_logger");
	}
	
	// check for invalid bucket size
	if (_batch_size < 1)
	{
		_logger->warn("Batch size of {} is invalid. changing to default of 5000", _batch_size);
		_batch_size = 5000;
	}

	_logger->info("Influx db client initialised with batch size of {}", _batch_size);


}


// no provided logger
InfluxDBClient::InfluxDBClient
( const std::string& url
, const std::string& org
, const std::string& token
, int batch_size
)
: InfluxDBClient(url, token, org, batch_size, spdlog::null_logger_mt("null_logger"))
{}

// no provided batch size
InfluxDBClient::InfluxDBClient
( const std::string& url
, const std::string& org
, const std::string& token
, std::shared_ptr<spdlog::logger> logger
)
: InfluxDBClient(url, token, org, 5000, logger)
{}

// no provided logger or batch size
InfluxDBClient::InfluxDBClient
( const std::string& url
, const std::string& org
, const std::string& token
)
: InfluxDBClient(url, token, org, 5000, spdlog::null_logger_mt("null_logger"))
{}

}

}


