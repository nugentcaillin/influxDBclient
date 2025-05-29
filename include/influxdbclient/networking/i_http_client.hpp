#ifndef _I_HTTP_CLIENT_H_
#define _I_HTTP_CLIENT_H_


#include <string>
#include <map>
#include "influxdbclient/networking/http_request.hpp"
#include "influxdbclient/networking/http_response.hpp"
#include "influxdbclient/networking/task.hpp"

namespace influxdbclient
{
namespace networking
{




class IHttpClient
{
public:
	virtual ~IHttpClient() = default;

	virtual Task<HttpResponse> performAsync(HttpRequest& request) = 0;
};

}
}

#endif
