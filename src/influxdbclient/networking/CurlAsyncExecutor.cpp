#include "influxdbclient/networking/curl_awaitable.hpp"
#include "influxdbclient/networking/curl_async_executor.hpp"
#include "curl_global_initializer.hpp"
#include <curl/curl.h>
#include <thread>
#include <iostream>
#include <mutex>
#include <chrono>
#include <condition_variable>
#include <coroutine>
#include <map>
#include <stdexcept>


namespace influxdbclient
{
namespace networking
{


static CurlGlobalInitializer curlGlobalInitializer;



CurlAsyncExecutor::CurlAsyncExecutor()
{
	_multi_handle = curl_multi_init();	
	std::cout << "curl_multi_init called" << std::endl; 
	_running = true;
	_io_thread = std::thread(&CurlAsyncExecutor::run, this);
	std::cout << "thread started" << std::endl;
}

CurlAsyncExecutor::~CurlAsyncExecutor()
{
	_running = false;
	_action_cv.notify_one();
	if (_io_thread.joinable()) _io_thread.join();
	curl_multi_cleanup(_multi_handle);
	_multi_handle = nullptr;
	std::cout << "curl_multi_cleanup called" << std::endl; 

}

CurlAwaitable CurlAsyncExecutor::queueRequest(CURL *easy_handle) {
	return CurlAwaitable(easy_handle, *this);
}

// we need the executor to own the request state for libcurl to write
CurlAsyncExecutor::RequestState *
CurlAsyncExecutor::registerRequest
( RequestState rs
)
{
	RequestState *ret;
	{
		std::lock_guard<std::mutex> lock(_mutex);
		// insert into map, giving map ownership of RS
		auto [it, inserted] = _requests_map.emplace(rs.easy_handle, std::move(rs));

		if (!inserted)
		{
			std::runtime_error("handle already registered");
		}

		// now add handle to queue

		_handle_queue.push(it->second.easy_handle);
		ret = &(it->second);
	}
	_action_cv.notify_one();
	return ret;
}


void CurlAsyncExecutor::run()
{
	std::cout << "Event loop started, size: " << _handle_queue.size() << std::endl;
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

			std::cout << "blocking until requests queued, requests done or not running with timeout of: " << timeout.count() << " and " << total_msgs << "messages" << std::endl;
			std::unique_lock<std::mutex> lock(_mutex);
			// if no requests pending, wait until new request or interrupt
			_action_cv.wait_for(lock, timeout, [this] {
				return !_running || !_handle_queue.empty();
			});

			if (!_running) break;

			if (!_handle_queue.empty()) std::cout << "handling queue items" << std::endl;
			

			// add new handles
			while (!_handle_queue.empty())
			{
				auto curr = _handle_queue.front();
				_handle_queue.pop();
				curl_multi_add_handle(_multi_handle, curr);
				total_msgs++;
			}
		}
		std::cout << "done blocking, dealing with transfers" << std::endl;

		// perform transfers
		mc = curl_multi_perform(_multi_handle, &still_running);
		if (mc != CURLM_OK)
		{
			std::cout << "error in curl_multi_perform(): " << curl_multi_strerror(mc) << std::endl;
		}

		// deal with completed transfers
		while (msg = curl_multi_info_read(_multi_handle, &msgs_in_queue))
		{
			if (msg->msg == CURLMSG_DONE)
			{
				total_msgs--;
				if (msg->data.result == CURLE_OK)
				{
					long response_code;
					curl_easy_getinfo(msg->easy_handle, CURLINFO_RESPONSE_CODE, &response_code);
					std::cout << "transfer done with code: " << response_code << std::endl;


				} else 
				{
					std::cout << "transfer completed with error: " << curl_easy_strerror(msg->data.result) << std::endl;
				}

				auto res = _requests_map.find(msg->easy_handle); 

				curl_multi_remove_handle(_multi_handle, msg->easy_handle);

				std::cout << "about to resume" << std::endl;

				res->second.awaiting_coroutine.resume();
				
				_requests_map.erase(res);

				// before adding processing thread, safe to remove from map here

			}
			else 
			{
				std::cout << "other event on message" << std::endl;
			}
		}

	}
	// remove any queued requests
	while (!_handle_queue.empty())
	{
		std::cout << "cheaning up queue item" << std::endl;
		CURL *handle = _handle_queue.front();
		_handle_queue.pop();
		curl_easy_cleanup(handle);
	}


	// chean up any current requests
	/*
	CURL **easy_handles = curl_multi_get_handles(_multi_handle);
	
	while (*easy_handles)
	{
		std::cout << "cheaning up ongoing request" << std::endl;
		curl_multi_remove_handle(_multi_handle, *easy_handles);
		curl_easy_cleanup(*easy_handles);
		easy_handles++;
	}
	curl_free(easy_handles);
	*/

	std::cout << "thread closing" << std::endl;
}

CurlAsyncExecutor& CurlAsyncExecutor::getInstance()
{
	static CurlAsyncExecutor instance;
	return instance;
}

}
}
