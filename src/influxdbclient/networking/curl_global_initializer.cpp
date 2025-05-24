#include <mutex>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/null_sink.h>
#include <memory>
#include <curl/curl.h>
#include <stdexcept>
#include "curl_global_initializer.hpp"
#include <iostream>

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
		CURLcode res = curl_global_init(CURL_GLOBAL_ALL);

		if (res != CURLE_OK)
		{
			std::runtime_error(std::string(curl_easy_strerror(res)));
		}
	}
}

CurlGlobalInitializer::~CurlGlobalInitializer()
{
	std::lock_guard<std::mutex> lock(_mutex);
	if (--_ref_count == 0)
	{
		curl_global_cleanup();
	}
}




}
}
