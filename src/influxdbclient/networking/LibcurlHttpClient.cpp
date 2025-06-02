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


	
UniqueCurlSlist
LibcurlHttpClient::buildHeaderSlist
( const std::map<std::string, std::string>& headers)
{
	curl_slist *headers_list = nullptr;
	curl_slist *new_list;
	for (auto it = headers.begin(); it != headers.end(); it++)
	{
		std::string curr;
		curr += it->first;
		curr += ": ";
		curr += it->second;
	new_list = curl_slist_append(headers_list, curr.c_str());
		if (!new_list)
		{
			if (headers_list) curl_slist_free_all(headers_list);
			throw std::runtime_error("out of memory assigning header: " + curr);
		}
		headers_list = new_list;
	}
	return UniqueCurlSlist(headers_list);
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
	

	CURL *easy_handle = curl_easy_init();
	
	if (!easy_handle)
	{
		throw std::runtime_error("unable to create CURL handle");
	}

	// deal with headers
	UniqueCurlSlist headers = buildHeaderSlist(request.getHeaders());
	curl_easy_setopt(easy_handle, CURLOPT_HTTPHEADER, headers.get());
	
	
	// url
	curl_easy_setopt(easy_handle, CURLOPT_URL, request.getUrl().c_str());

	// set body
	if (request.getBody() != "")
	{
		curl_easy_setopt(easy_handle, CURLOPT_COPYPOSTFIELDS, request.getBody().c_str());
	}
	// deal with request type
	switch(request.getMethod())
	{
		case HttpMethod::GET:
			curl_easy_setopt(easy_handle, CURLOPT_HTTPGET, 1L);
			break;
		case HttpMethod::POST:
			curl_easy_setopt(easy_handle, CURLOPT_POST, 1L);
			break;
		default:
			throw std::runtime_error("unsupported http method");
	}
	
	
	// construct requestState
	auto rs = std::make_unique<RequestState>();
	rs->easy_handle = easy_handle;
	rs->headers = std::move(headers);

	if (!rs->easy_handle)
	{
		throw std::runtime_error("failed to initialize curl easy pointer");
	}
	
	std::cout << "making promise and future" << std::endl;
	std::optional<std::promise<std::unique_ptr<RequestState>>> requestStateOptionalPromise;
	std::optional<std::future<std::unique_ptr<RequestState>>> requestStateOptionalFuture;

	requestStateOptionalPromise.emplace();
	requestStateOptionalFuture.emplace(requestStateOptionalPromise->get_future());


	// queue our request
	std::cout << "performing async request" << std::endl;

	std::cout << "waiting" << std::endl;	
	co_await CurlAwaitable(std::move(rs), std::ref(*requestStateOptionalPromise));
	//std::cout << completed_rs->body << std::endl;

	std::cout << "Async request finished" << std::endl;
	
	std::cout << "constructing HttpResponse" << std::endl;

	std::unique_ptr<RequestState> completed_rs = std::move(requestStateOptionalFuture->get());
	
	requestStateOptionalPromise.reset();
	requestStateOptionalFuture.reset();

	HttpResponse response(
		//std::move(completed_rs),
		"foo", 
		//std::move(completed_rs->body),
		200,
		//completed_rs->http_status,
		CURLE_OK 
		//completed_rs->curl_code
	);
	
	std::cout << "about to resume" << std::endl;

	co_return std::move(response);
	
} 


}
}
