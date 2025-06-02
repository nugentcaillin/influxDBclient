#ifndef _CURL_AWAITABLE_H_
#define _CURL_AWAITABLE_H_

#include <curl/curl.h>
#include "influxdbclient/networking/http_response.hpp"
#include "influxdbclient/networking/http_request.hpp"
#include <coroutine>
#include <future>
#include "RequestState.hpp"

namespace influxdbclient
{
namespace networking
{

//struct RequestState;

class CurlAwaitable {
public:

	
	CurlAwaitable
	(HttpRequest& request, std::promise<HttpResponse>& promise)
	: _request(request)
	, _promise(promise)
	{}

	bool
	await_ready
	()
	{
		return false;
	}

	void
	await_suspend
	( std::coroutine_handle<> h
	);
	void
	await_resume
	();
private:
	HttpRequest& _request;
	std::promise<HttpResponse>& _promise;
};

}
}

#endif
