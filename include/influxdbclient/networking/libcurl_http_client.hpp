#ifndef _LIBCURL_HTTP_CLIENT_H_
#define _LIBCURL_HTTP_CLIENT_H_

#include "influxdbclient/networking/i_http_client.hpp"
#include "influxdbclient/networking/task.hpp"
#include "influxdbclient/networking/curl_async_executor.hpp"
#include "influxdbclient/networking/http_response.hpp"
#include "influxdbclient/networking/http_request.hpp"
#include "curl/curl.h"
#include "slist_unique_ptr.hpp"
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

	Task<HttpResponse> performAsync(HttpRequest &request);

};


}
}

#endif
