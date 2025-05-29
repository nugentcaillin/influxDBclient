#ifndef _REQUEST_STATE_H_
#define _REQUEST_STATE_H_

#include "slist_unique_ptr.hpp"
#include <coroutine>

namespace influxdbclient
{
namespace networking
{

struct RequestState {
	CURL *easy_handle;
	long http_status = 0;
	CURLcode curl_code = CURLE_OK;
	std::string body;
	UniqueCurlSlist headers;
	std::promise<HttpResponse> promise;
	std::coroutine_handle<> awaiting_coroutine;

	~RequestState() {
		curl_easy_cleanup(easy_handle);
	}
};



} // namespace networking
} // namespace influxdbclient

#endif // _REQUEST_STATE_H_
