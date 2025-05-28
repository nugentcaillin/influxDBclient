#ifndef _I_HTTP_CLIENT_H_
#define _I_HTTP_CLIENT_H_


#include <string>
#include <map>
#include "influxdbclient/networking/http_response.hpp"

namespace influxdbclient
{
namespace networking
{




class IHttpClient
{
public:
	virtual ~IHttpClient() = default;
	virtual HttpResponse post(const std::string& url, const std::string& body, const std::map<std::string, std::string>) = 0;
	virtual HttpResponse get(const std::string& url, const std::string& body, const std::map<std::string, std::string>) = 0;
};

}
}

#endif
