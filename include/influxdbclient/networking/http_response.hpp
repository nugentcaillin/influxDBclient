#ifndef _HTTP_RESPONSE_H_
#define _HTTP_RESPONSE_H_

#include <string>
#include <curl/curl.h>
#include "RequestState.hpp"

namespace influxdbclient
{
namespace networking
{

class HttpResponse
{
private:
public:
	std::string body;
	long http_status;
	CURLcode curl_code;

public:
	HttpResponse
	(std::string body_val
	, long status_val
	, CURLcode curl_code_val)
	: body(std::move(body_val))
	, http_status(status_val)
	, curl_code(curl_code_val)
	{}
	HttpResponse(HttpResponse&& other) noexcept = default;
	HttpResponse& operator=(HttpResponse&& other) noexcept = default;

	HttpResponse(const HttpResponse&) = delete;
	HttpResponse& operator=(const HttpResponse&) = delete;
};

}
}

#endif
