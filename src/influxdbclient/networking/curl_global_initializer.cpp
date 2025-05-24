#include <mutex>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/null_sink.h>
#include <memory>
#include <curl/curl.h>
#include <stdexcept>
#include "curl_global_initializer.hpp"

namespace influxdbclient
{
namespace networking
{

int CurlGlobalInitializer::_ref_count = 0;
std::mutex CurlGlobalInitializer::_mutex;


CurlGlobalInitializer::CurlGlobalInitializer()
{
	std::lock_guard<std::mutex> lock(_mutex);
	if (_ref_count++ == 0)
	{
		// attempt to get global logger, otherwise null sink
		_logger = spdlog::get("influx_db_client_global_logger");
		if (_logger.get() == nullptr)
		{
			_logger = spdlog::null_logger_mt("influx_db_client_global_logger");
		}
		CURLcode res = curl_global_init(CURL_GLOBAL_ALL);

		if (res != CURLE_OK)
		{
			_logger->error("Curl unable to initialize");
			std::runtime_error(std::string(curl_easy_strerror(res)));
		}
		_logger->info("curl initialized");
	}
}

CurlGlobalInitializer::~CurlGlobalInitializer()
{
	std::lock_guard<std::mutex> lock(_mutex);
	if (--_ref_count == 0)
	{
		curl_global_cleanup();
		_logger->info("curl cleaned up");
	}
}




}
}
