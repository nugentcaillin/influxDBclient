#ifndef _CURL_ASYNC_EXECUTOR_H
#define _CURL_ASYNC_EXECUTOR_H

#include <curl/curl.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <coroutine>
#include <map>
#include <string>


namespace influxdbclient
{
namespace networking
{


struct RequestState;

class CurlAsyncExecutor
{

public:
	struct ActiveRequest {
		std::unique_ptr<RequestState> rs;
		std::function<void(std::unique_ptr<RequestState> rs_result)> completion_callback;
		std::coroutine_handle<> continuation;
	};
	static CurlAsyncExecutor& getInstance();	
		
	
	void
	queueRequest(std::unique_ptr<RequestState> rs, std::function<void(std::unique_ptr<RequestState> rs_result)>, std::coroutine_handle<> continuation);

	static size_t writeCallback(char *contents, size_t size, size_t nmemb, void *userdata);


private:
	bool _running = false;
	std::mutex _mutex;
	std::thread _io_thread;
	CURLM *_multi_handle;
	std::condition_variable _action_cv;
	std::queue<CURL *> _handle_queue;
	std::map<CURL *, ActiveRequest> _requests_map;

	CurlAsyncExecutor();
	~CurlAsyncExecutor();
	void run();

};


}
}

#endif
