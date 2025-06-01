#include "influxdbclient/networking/curl_awaitable.hpp"
#include <curl/curl.h>
#include "influxdbclient/networking/curl_async_executor.hpp"
#include "influxdbclient/networking/http_response.hpp"
#include <coroutine>
#include <iostream>
#include <stdexcept>
#include "RequestState.hpp"


namespace influxdbclient
{
namespace networking
{

void
CurlAwaitable::await_suspend
( std::coroutine_handle<> h
)
{
	if (!_rs_ptr) throw std::runtime_error("null requeststate pointer");
	_rs_ptr->awaiting_coroutine = h;
	CurlAsyncExecutor::getInstance().queueRequest(std::move(_rs_ptr));
}

HttpResponse
CurlAwaitable::await_resume
()
{
	return _future.get();
}

}
}

