#include "influxdbclient/networking/http_request.hpp"
#include <string>
#include <map>

namespace influxdbclient
{
namespace networking
{

void
HttpRequest::setUrl
(std::string url)
{
	_url = std::move(url);
}

void
HttpRequest::setBody
(std::string body)
{
	_body = std::move(body);
}

void
HttpRequest::setMethod
(HttpMethod method)
{
	_method = method;
}

void
HttpRequest::addHeader
(std::string key, std::string value)
{
	// deal with duplicates here
	_headers.emplace(std::move(key), std::move(value));
}


} // namespace networking	
} // namespace influxdbclient
