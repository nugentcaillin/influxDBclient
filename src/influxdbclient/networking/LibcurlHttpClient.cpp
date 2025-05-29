#include "influxdbclient/networking/libcurl_http_client.hpp"
#include "influxdbclient/networking/curl_async_executor.hpp"
#include "influxdbclient/networking/task.hpp"
#include "influxdbclient/networking/curl_awaitable.hpp"
#include "curl_global_initializer.hpp"
#include "influxdbclient/networking/http_response.hpp"
#include <curl/curl.h>
#include <iostream>

namespace influxdbclient
{
namespace networking
{

static CurlGlobalInitializer curlGlobalInitializer;

LibcurlHttpClient::~LibcurlHttpClient() = default;

LibcurlHttpClient::LibcurlHttpClient()
{
}

HttpResponse LibcurlHttpClient::post(const std::string& url, const std::string& body, const std::map<std::string, std::string>)
{
	return {};
}
HttpResponse LibcurlHttpClient::get(const std::string& url, const std::string& body, const std::map<std::string, std::string>)
{
	return {};
}




Task<void> 
LibcurlHttpClient::test
( std::string url
)
{
	std::cout << "creating request" << std::endl;
	CURL* easy_handle = curl_easy_init();
	curl_easy_setopt(easy_handle, CURLOPT_URL, url.c_str());
	std::cout << "suspending for first time" << std::endl;
	HttpResponse res = co_await CurlAsyncExecutor::getInstance().queueRequest(easy_handle);

	std::cout << "resuming main coroutine with status " << res.status << std::endl;
	std::cout << res.body << std::endl;
}


}
}
