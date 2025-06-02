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
	if (!_rs_ptr->easy_handle) throw std::runtime_error("null easy_handle");

	std::function<void(std::unique_ptr<RequestState> rs_result)> completion_callback = [this](std::unique_ptr<RequestState> rs_result) {
		this->_promise.set_value(std::move(rs_result));

	};
	std::cout << "queueing request" << std::endl;
	CurlAsyncExecutor::getInstance().queueRequest(std::move(_rs_ptr), completion_callback, h);
}

void
CurlAwaitable::await_resume
()
{

}

}
}

