#ifndef _REQUEST_STATE_H_
#define _REQUEST_STATE_H_

#include "slist_unique_ptr.hpp"
#include <coroutine>
#include <iostream>
#include <future>
#include <utility>
#include <curl/curl.h>

namespace influxdbclient
{
namespace networking
{

class HttpResponse;

struct RequestState {
	CURL *easy_handle;
	long http_status = 0;
	CURLcode curl_code = CURLE_OK;
	std::string body;
	UniqueCurlSlist headers;

	~RequestState() {
		std::cout << "requeststate destructor" << std::endl;

		if (easy_handle)
		{
			std::cout << "doing cleanup on handle" << std::endl;
			curl_easy_cleanup(easy_handle);
		}
		if (headers)
		{
			std::cout << "we have a reference to headers" << std::endl;
		}
	}
	RequestState()
	: easy_handle(nullptr)
	, http_status(0)
	, curl_code(CURLE_OK)
	, body()
	, headers()
	{}


	// move only due to unique ptrs
	RequestState(const RequestState&) = delete;
	RequestState& operator=(const RequestState&) = delete;

	RequestState(RequestState&& other) noexcept
	: easy_handle(std::exchange(other.easy_handle, nullptr))
	, http_status(std::exchange(other.http_status, 0))
	, curl_code(std::exchange(other.curl_code, CURLE_OK))
	, body(std::move(other.body))
	, headers(std::move(other.headers))
	{}

	RequestState& operator=(RequestState&& other)
	{
		if (this != &other)
		{
			if (easy_handle) {
				curl_easy_cleanup(easy_handle);
			}
			easy_handle = std::exchange(other.easy_handle, nullptr);
			http_status = std::exchange(other.http_status, 0);
			curl_code = std::exchange(other.curl_code, CURLE_OK);
			body = std::move(other.body);
			headers = std::move(other.headers);
		}
		return *this;
	}
};



} // namespace networking
} // namespace influxdbclient

#endif // _REQUEST_STATE_H_
