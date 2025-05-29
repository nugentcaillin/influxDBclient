#include "influxdbclient/networking/libcurl_http_client.hpp"
#include "influxdbclient/networking/curl_async_executor.hpp"
#include "influxdbclient/networking/task.hpp"
#include "influxdbclient/networking/curl_awaitable.hpp"
#include "curl_global_initializer.hpp"
#include "influxdbclient/networking/http_response.hpp"
#include "influxdbclient/networking/http_request.hpp"
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





Task<HttpResponse> 
LibcurlHttpClient::performAsync
( HttpRequest& request
)
{
	
}


}
}
