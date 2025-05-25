#include "influxdbclient/networking/curl_async_executor.hpp"
#include "curl_global_initializer.hpp"
#include <curl/curl.h>
#include <thread>
#include <iostream>
#include <mutex>
#include <chrono>
#include <condition_variable>


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

void CurlAsyncExecutor::submit_handle(CURL *handle)
{
	std::cout << "submitting handle" << std::endl;
	{
		std::unique_lock<std::mutex> lock(_mutex);
		_handle_queue.push(handle);
	}
	_action_cv.notify_one();
}

void CurlAsyncExecutor::run()
{
	std::cout << "Event loop started, size: " << _handle_queue.size() << std::endl;

	while (_running)
	{
		{
			std::cout << "waiting for request or end" << std::endl;
			// wait until queue has items or 
			std::unique_lock<std::mutex> lock(_mutex);
			_action_cv.wait(lock, [this] {
				return !_running || !_handle_queue.empty();
			});

			if (!_running) break;

			std::cout << "handling queue items" << std::endl;

			while (!_handle_queue.empty())
			{
				auto curr = _handle_queue.front();
				_handle_queue.pop();
				curl_easy_cleanup(curr);
			}
		}

	}
	std::cout << "size: " << _handle_queue.size() << std::endl;
	std::cout << "thread closing" << std::endl;
}

CurlAsyncExecutor& CurlAsyncExecutor::getInstance()
{
	static CurlAsyncExecutor instance;
	return instance;
}

}
}
