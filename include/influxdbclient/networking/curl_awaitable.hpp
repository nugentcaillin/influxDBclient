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
	(std::unique_ptr<RequestState> rs_ptr, std::promise<std::unique_ptr<RequestState>>& promise)
	: _rs_ptr(std::move(rs_ptr))
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
	std::unique_ptr<RequestState> _rs_ptr;
	std::promise<std::unique_ptr<RequestState>>& _promise;
};

}
}

#endif
