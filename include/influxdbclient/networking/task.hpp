#ifndef _TASK_H_
#define _TASK_H_


#include <coroutine>
#include <exception>
#include <iostream>
#include <future>
#include <utility>
#include <optional>

namespace influxdbclient
{
namespace networking
{

template<typename T>
class Task {
public:
	Task(const Task&) = delete;
	Task& operator=(const Task&) = delete;
	Task(Task&& other) noexcept : _handle(std::exchange(other._handle, nullptr)), 
								_future(std::move(other._future)) {}
	Task& operator=(Task&& other) noexcept {
		if (this != &other) {
			if (_handle) {
				_handle.destroy();
			}
			_handle = std::exchange(other._handle, nullptr);
			_future = std::move(other._future);
		}
		return *this;
	}


	struct promise_type {
		std::promise<T> final_promise;
		std::coroutine_handle<> continuation;

		Task<T> get_return_object() {
			return Task<T>(std::coroutine_handle<promise_type>::from_promise(*this));
		}

		std::suspend_always initial_suspend() {return {};}

		std::suspend_always final_suspend() noexcept { 
			if (continuation)
			{
				continuation.resume();
			}
			return {};
		}

		void return_value(T value_) { 
			final_promise.set_value(std::move(value_)); 
		}
		void unhandled_exception() { 
			final_promise.set_exception(std::current_exception()); 
		}

	};
	

	T get()
	{
		if (!_handle)
		{
			throw std::runtime_error("no handle");
		}
		

		if (!_handle.done()) _handle.resume();


		return std::move(_future.get());
	}

	Task(std::coroutine_handle<promise_type> h) : _handle(h) {
		if (_handle)
		{
			_future = _handle.promise().final_promise.get_future();
		}
	}

	~Task() {
		std::cout << "Task destructor" << std::endl;
		if (_handle)
		{
			_handle.destroy();
		}
	}

	bool await_ready() { return !_handle || _handle.done(); }
	void await_suspend(std::coroutine_handle<> continuation_) {
		_handle.promise().continuation = continuation_; 
		_handle.resume();
	}
	T await_resume() {
		T result = std::move(_future.get());
		//if (_handle) _handle.destroy();
		return std::move(result);
	}


	std::coroutine_handle<promise_type> _handle;
	std::future<T> _future;

};


template<>
class Task<void> {
public:

	Task(const Task&) = delete;
	Task& operator=(const Task&) = delete;
	
	Task(Task&& other) noexcept : _handle(std::exchange(other._handle, nullptr)),
								_future(std::move(other._future)) {}
	Task& operator=(Task&& other) noexcept {
		if (this != &other) {
			if (_handle) {
				_handle.destroy();
			}
			_handle = std::exchange(other._handle, nullptr);
			_future = std::move(other._future);
		}
		return *this;
	}

	~Task()
	{
		if (_handle) _handle.destroy();
	}

	struct promise_type {
		std::coroutine_handle<> continuation;
		std::promise<void> final_promise;

		Task<void> get_return_object() {
			return Task<void>(std::coroutine_handle<promise_type>::from_promise(*this));
		}

		std::suspend_always initial_suspend() {return {};}

		std::suspend_always final_suspend() noexcept { 
			if (continuation)
			{
				continuation.resume();
			}
			return {};
		}

		void return_void() { final_promise.set_value(); }
		void unhandled_exception() { 
			final_promise.set_exception(std::current_exception());
		}
	};
	
	void get()
	{
		if (!_handle) throw std::runtime_error("no handle");
		return _future.get();
	}



	Task(std::coroutine_handle<promise_type> h) : _handle(h) {
		if (_handle) _future = _handle.promise().final_promise.get_future();
	}


	bool await_ready() { return !_handle || _handle.done(); }
	void await_suspend(std::coroutine_handle<> continuation_) {
		_handle.promise().continuation = continuation_;
		_handle.resume();
	}
	void await_resume() {
		_future.get();
	}



	std::coroutine_handle<promise_type> _handle;
	std::future<void> _future;

};




}
}
#endif
