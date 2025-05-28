#ifndef _CURL_ASYNC_EXECUTOR_H
#define _CURL_ASYNC_EXECUTOR_H

#include <curl/curl.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <coroutine>
#include <map>


namespace influxdbclient
{
namespace networking
{


class CurlAwaitable;

class CurlAsyncExecutor
{
public:
	static CurlAsyncExecutor& getInstance();	
	
	struct RequestState {
		CURL *easy_handle;
		std::coroutine_handle<> awaiting_coroutine;
		long http_status_code = 0;
		CURLcode curl_code = CURLE_OK;
	};

	CurlAwaitable queueRequest(CURL *easy_handle);
	RequestState *registerRequest(RequestState rs);


private:
	bool _running = false;
	std::mutex _mutex;
	std::thread _io_thread;
	CURLM *_multi_handle;
	std::condition_variable _action_cv;
	std::queue<CURL *> _handle_queue;
	std::map<CURL *, RequestState> _requests_map;

	CurlAsyncExecutor();
	~CurlAsyncExecutor();
	void run();

};


}
}

#endif
