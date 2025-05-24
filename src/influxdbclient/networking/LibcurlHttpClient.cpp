#include "influxdbclient/networking/libcurl_http_client.hpp"

#include "curl_global_initializer.hpp"

namespace influxdbclient
{
namespace networking
{

static CurlGlobalInitializer curlGlobalInitializer; 

HttpResponse LibcurlHttpClient::post(const std::string& url, const std::string& body, const std::map<std::string, std::string>)
{
	return {0, "", {}};
}
HttpResponse LibcurlHttpClient::get(const std::string& url, const std::string& body, const std::map<std::string, std::string>)
{
	return {0, "", {}};
}

}
}
