#include "influxdbclient/networking/curl_awaitable.hpp"
#include "influxdbclient/networking/curl_async_executor.hpp"
#include "curl_global_initializer.hpp"
#include <curl/curl.h>
#include <thread>
#include <iostream>
#include <mutex>
#include <chrono>
#include <memory>
#include <condition_variable>
#include <coroutine>
#include <map>
#include <stdexcept>
#include <string>
#include "slist_unique_ptr.hpp"
#include "RequestState.hpp"


namespace influxdbclient
{
namespace networking
{


static CurlGlobalInitializer curlGlobalInitializer;



CurlAsyncExecutor::CurlAsyncExecutor()
{
	_multi_handle = curl_multi_init();	
	_running = true;
	_io_thread = std::thread(&CurlAsyncExecutor::run, this);
}

CurlAsyncExecutor::~CurlAsyncExecutor()
{
	_running = false;
	_action_cv.notify_one();
	if (_io_thread.joinable()) _io_thread.join();
	std::cout << "curl multi cleanup called" << std::endl;
	curl_multi_cleanup(_multi_handle);
	_multi_handle = nullptr;

}
void
CurlAsyncExecutor::queueRequest(std::unique_ptr<RequestState> rs, std::function<void(std::unique_ptr<RequestState> rs_result)> completion_callback, std::coroutine_handle<> continuation)
{
	std::cout << "queueing req in CAE" << std::endl;
	if (!rs->easy_handle) throw std::runtime_error("easy_handle given to queue request is null");

	{
		std::lock_guard<std::mutex> lock(_mutex);
		
		// set libcurl callbacks
		curl_easy_setopt(rs->easy_handle, CURLOPT_WRITEFUNCTION, CurlAsyncExecutor::writeCallback);
		curl_easy_setopt(rs->easy_handle, CURLOPT_WRITEDATA, (void *) &(rs->body));
		CURL* easy_handle = rs->easy_handle;

		// place in map, and queue request
		_handle_queue.push(easy_handle);

		ActiveRequest ar;
		ar.rs = std::move(rs);
		ar.completion_callback = completion_callback;
		ar.continuation = continuation;



		std::cout << "emplacing in map" << std::endl;
		_requests_map.emplace(easy_handle, std::move(ar));
		
	}
	std::cout << "done queueing req in CAE" << std::endl;
	_action_cv.notify_one();
}


void CurlAsyncExecutor::run()
{
	int still_running;
	int msgs_in_queue;
	int total_msgs = 0;
	std::chrono::milliseconds timeout;
	CURLMcode mc;
	CURLMsg *msg;

	while (_running)
	{
		{
			// if no requests, set timeout to max, otherwise curl's suggested timeout or default

			if (total_msgs != 0)
			{
				long timeout_long;
				curl_multi_timeout(_multi_handle, &timeout_long);
				if (timeout_long == -1) timeout_long = 50;
				timeout = std::chrono::milliseconds(timeout_long);
			} else 
			{
				timeout = std::chrono::minutes(10); // not using std::chrono::milliseconds::max due to overflow
			}

			std::unique_lock<std::mutex> lock(_mutex);
			// if no requests pending, wait until new request or interrupt
			_action_cv.wait_for(lock, timeout, [this] {
				return !_running || !_handle_queue.empty();
			});

			if (!_running) break;

			

			// add new handles
			while (!_handle_queue.empty())
			{
				auto curr = _handle_queue.front();
				_handle_queue.pop();
				curl_multi_add_handle(_multi_handle, curr);
				total_msgs++;
			}
		}

		// perform transfers
		mc = curl_multi_perform(_multi_handle, &still_running);

		// add error check here

		// deal with completed transfers
		while (msg = curl_multi_info_read(_multi_handle, &msgs_in_queue))
		{
			if (msg->msg == CURLMSG_DONE)
			{
				total_msgs--;
				std::unique_ptr<RequestState> completed_rs;
				std::coroutine_handle<> continuation;
				std::function<void(std::unique_ptr<RequestState> completed_rs)> completion_callback;
				{
					std::lock_guard<std::mutex> lock(_mutex);
					auto it = _requests_map.find(msg->easy_handle);
					if (it != _requests_map.end())
					{
						completed_rs = std::move(it->second.rs);
						completion_callback = it->second.completion_callback;
						continuation = it->second.continuation;
						_requests_map.erase(it);
						curl_multi_remove_handle(_multi_handle, completed_rs->easy_handle);
					} else 
					{
						throw std::runtime_error("request not found in map");
					}
				}

				
				// get these reference before setting promise

				completed_rs->curl_code = msg->data.result;
				
				std::cout << "RESUMING COROUTINE" << std::endl;
				completion_callback(std::move(completed_rs));
				continuation.resume();

				//completed_rs->awaiting_coroutine.resume();

			}
		}

	}
	// remove any queued requests
	while (!_handle_queue.empty())
	{
		CURL *handle = _handle_queue.front();
		_handle_queue.pop();
		curl_easy_cleanup(handle);
	}


	// chean up any current requests
	/*
	CURL **easy_handles = curl_multi_get_handles(_multi_handle);
	
	while (*easy_handles)
	{
		curl_multi_remove_handle(_multi_handle, *easy_handles);
		curl_easy_cleanup(*easy_handles);
		easy_handles++;
	}
	curl_free(easy_handles);
	*/

}

CurlAsyncExecutor& CurlAsyncExecutor::getInstance()
{
	static CurlAsyncExecutor instance;
	return instance;
}

size_t
CurlAsyncExecutor::writeCallback(char *contents, size_t size, size_t nmemb, void *userdata)
{
	((std::string *) userdata)->append(contents, size * nmemb);
	return size * nmemb;
}

}
}
