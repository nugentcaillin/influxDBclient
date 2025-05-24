#ifndef _LIBCURL_HTTP_CLIENT_H_
#define _LIBCURL_HTTP_CLIENT_H_

#include "influxdbclient/networking/i_http_client.hpp"
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

	HttpResponse post(const std::string& url, const std::string& body, const std::map<std::string, std::string>) override;
	HttpResponse get(const std::string& url, const std::string& body, const std::map<std::string, std::string>) override;
};


}
}

#endif
