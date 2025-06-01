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
	curl_multi_cleanup(_multi_handle);
	_multi_handle = nullptr;

}
void
CurlAsyncExecutor::queueRequest(std::unique_ptr<RequestState> rs) {
	{
		std::lock_guard<std::mutex> lock(_mutex);
		
		// set libcurl callbacks
		curl_easy_setopt(rs->easy_handle, CURLOPT_WRITEFUNCTION, CurlAsyncExecutor::writeCallback);
		curl_easy_setopt(rs->easy_handle, CURLOPT_WRITEDATA, (void *) &(rs->body));

		// place in map, and queue request
		_handle_queue.push(rs->easy_handle);
		_requests_map.emplace(rs->easy_handle, std::move(rs));
		
	}
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
				{
					std::lock_guard<std::mutex> lock(_mutex);
					auto it = _requests_map.find(msg->easy_handle);
					if (it != _requests_map.end())
					{
						completed_rs = std::move(it->second);
						_requests_map.erase(it);
						curl_multi_remove_handle(_multi_handle, completed_rs->easy_handle);
					} else 
					{
						throw std::runtime_error("request not found in map");
					}
				}




				
				//add curlcode and status to response
				long http_status = 0;
				auto code = curl_easy_getinfo(msg->easy_handle, CURLINFO_RESPONSE_CODE, &http_status);
				if (code == CURLE_OK) completed_rs->http_status = http_status;

				completed_rs->http_status = http_status;
				completed_rs->curl_code = code;

				try {
					if (completed_rs->curl_code == CURLE_OK) {
						HttpResponse response;
						response.body = std::move(completed_rs->body);
						response.http_status = completed_rs->http_status;
						response.curl_code = completed_rs->curl_code;

						completed_rs->promise.set_value(std::move(response));
					} else {
						std::string err_str = "CURL error: ";
						err_str += curl_easy_strerror(completed_rs->curl_code);
						completed_rs->promise.set_exception(std::make_exception_ptr(std::runtime_error(err_str)));
					}
				} catch (...)
				{
					completed_rs->promise.set_exception(std::current_exception());
				}
				completed_rs->awaiting_coroutine.resume();

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
