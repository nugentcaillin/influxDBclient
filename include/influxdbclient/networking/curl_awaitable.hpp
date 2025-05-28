#ifndef _CURL_AWAITABLE_H_
#define _CURL_AWAITABLE_H_

#include <curl/curl.h>
#include "influxdbclient/networking/curl_async_executor.hpp"
#include "influxdbclient/networking/http_response.hpp"
#include <coroutine>

namespace influxdbclient
{
namespace networking
{


class CurlAwaitable {
public:

	
	CurlAwaitable
	( CURL* handle
	, CurlAsyncExecutor& exec
	)
	: _easy_handle(handle)
	, _executor(exec)
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
	CURL *_easy_handle;
	CurlAsyncExecutor& _executor;
	CurlAsyncExecutor::RequestState *_state_ptr;

};

}
}

#endif
