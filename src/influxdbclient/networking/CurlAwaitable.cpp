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

	std::function<void(HttpResponse response)> completion_callback = [this](HttpResponse response) {
		this->_promise.set_value(std::move(response));

	};
	std::cout << "queueing request" << std::endl;
	CurlAsyncExecutor::getInstance().queueRequest(_request, completion_callback, h);
}

void
CurlAwaitable::await_resume
()
{

}

}
}

