#ifndef _CURL_AWAITABLE_H_
#define _CURL_AWAITABLE_H_

#include <curl/curl.h>
#include "influxdbclient/networking/http_response.hpp"
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
	( std::shared_future<HttpResponse> future
	, std::unique_ptr<RequestState> rs_ptr)
	: _future(std::move(future))
	, _rs_ptr(std::move(rs_ptr))
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

	HttpResponse
	await_resume
	();
private:
	std::shared_future<HttpResponse> _future;
	std::unique_ptr<RequestState> _rs_ptr;
};

}
}

#endif
