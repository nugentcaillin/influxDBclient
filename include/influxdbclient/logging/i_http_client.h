#ifndef _I_HTTP_CLIENT_H_
#define _I_HTTP_CLIENT_H_


#include <string>
#include <map>

namespace influxdbclient
{
namespace networking
{



struct HttpResponse {
	int status;
	std::string body;
	std::map<std::string, std::string> headers;
}



class IHttpClient
{
	virtual ~IHttpClient = default;
	virtual HttpResponse post(const std::string& url, const std::string& body, const std::map<std::string, std::string>) = 0;
	virtual HttpResponse get(const std::string& url, const std::string& body, const std::map<std::string, std::string>) = 0;
}

}
}

#endif
