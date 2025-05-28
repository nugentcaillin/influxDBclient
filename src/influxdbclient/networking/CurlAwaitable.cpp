#include "influxdbclient/networking/curl_awaitable.hpp"
#include <curl/curl.h>
#include "influxdbclient/networking/curl_async_executor.hpp"
#include <coroutine>
#include <iostream>
#include <stdexcept>


namespace influxdbclient
{
namespace networking
{

void
CurlAwaitable::await_suspend
( std::coroutine_handle<> h
)
{
	std::cout << "suspending curlawaitable" << std::endl;
	CurlAsyncExecutor::RequestState rs;
	rs.easy_handle = _easy_handle;
	rs.awaiting_coroutine = h;

	_state_ptr = _executor.registerRequest(std::move(rs));
	
}

int
CurlAwaitable::await_resume
()
{
	std::cout << "resuming curlawaitable" << std::endl;
	if (!_state_ptr)
	{
		throw std::runtime_error("state pointer is null");
	}
	curl_easy_cleanup(_state_ptr->easy_handle);
	return -1;
}

}
}

