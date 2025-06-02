#include "influxdbclient/networking/libcurl_http_client.hpp"
#include "influxdbclient/networking/curl_async_executor.hpp"
#include "influxdbclient/networking/task.hpp"
#include "influxdbclient/networking/curl_awaitable.hpp"
#include "curl_global_initializer.hpp"
#include "influxdbclient/networking/http_response.hpp"
#include "influxdbclient/networking/http_request.hpp"
#include <curl/curl.h>
#include "slist_unique_ptr.hpp"
#include <iostream>
#include <stdexcept>
#include "RequestState.hpp"
#include "influxdbclient/utils/future_awaiter.hpp"
#include <optional>

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
( HttpRequest& request)
{


	// ensure necessary fields present
	if (request.getMethod() == HttpMethod::_UNSET)
	{
		throw std::runtime_error("no method supplied");
	}
	if (request.getUrl() == "")
	{
		throw std::runtime_error("no URL supplied");
	}
	
	
	std::cout << "making promise and future" << std::endl;
	std::optional<std::promise<HttpResponse>> responsePromise;
	std::optional<std::future<HttpResponse>> responseFuture;

	responsePromise.emplace();
	responseFuture.emplace(responsePromise->get_future());


	// queue our request
	std::cout << "performing async request" << std::endl;

	std::cout << "waiting" << std::endl;	
	co_await CurlAwaitable(request, std::ref(*responsePromise));
	//std::cout << completed_rs->body << std::endl;

	std::cout << "Async request finished" << std::endl;
	
	std::cout << "constructing HttpResponse" << std::endl;

	
	HttpResponse response = responseFuture->get();


	responsePromise.reset();
	responseFuture.reset();
	std::cout << "response.body.data(): " << static_cast<const void*>(response.body.data()) << std::endl;	
	
	std::cout << "about to resume" << std::endl;
	std::string detatchedBody = std::string(response.body);

	co_return HttpResponse(
		std::move(detatchedBody),
		response.http_status,
		response.curl_code
	);
	
} 


}
}
