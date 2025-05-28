#ifndef _LIBCURL_HTTP_CLIENT_H_
#define _LIBCURL_HTTP_CLIENT_H_

#include "influxdbclient/networking/i_http_client.hpp"
#include "influxdbclient/networking/task.hpp"
#include "influxdbclient/networking/curl_async_executor.hpp"
#include <string>
#include <map>

namespace influxdbclient
{
namespace networking
{


class LibcurlHttpClient : public IHttpClient
{
public:

	~LibcurlHttpClient() override;
	LibcurlHttpClient();

	HttpResponse post(const std::string& url, const std::string& body, const std::map<std::string, std::string>) override;
	HttpResponse get(const std::string& url, const std::string& body, const std::map<std::string, std::string>) override;
	Task<void> test(std::string url);
};


}
}

#endif
