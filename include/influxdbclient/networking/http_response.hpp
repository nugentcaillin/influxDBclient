#ifndef _HTTP_RESPONSE_H_
#define _HTTP_RESPONSE_H_

#include <string>
#include <curl/curl.h>

namespace influxdbclient
{
namespace networking
{

struct HttpResponse
{
	std::string body;
	long http_status;
	CURLcode curl_code;
};

}
}

#endif
