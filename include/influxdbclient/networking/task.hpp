#ifndef _TASK_H_
#define _TASK_H_


#include <coroutine>
#include <exception>
#include <iostream>
#include <future>
#include <utility>

namespace influxdbclient
{
namespace networking
{

template<typename T>
class Task {
public:
	Task(const Task&) = delete;
	Task& operator=(const Task&) = delete;
	Task(Task&& other) noexcept : _handle(std::exchange(other._handle, nullptr)) {}
	Task& operator=(Task&& other) noexcept {
		if (this != &other) {
			if (_handle) {
				_handle.destroy();
			}
			_handle = std::exchange(other._handle, nullptr);
		}
		return *this;
	}


	struct promise_type {
		T value;
		std::exception_ptr exception_ptr;
		std::coroutine_handle<> continuation;
		std::promise<T> promise;

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
			value = value_; 
			promise.set_value(value_);
		}
		void unhandled_exception() { 
			exception_ptr = std::current_exception(); 
			promise.set_exception(exception_ptr);
		}
	};


	Task(std::coroutine_handle<promise_type> h) : _handle(h) {}

	~Task() {
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

		if (_handle.promise().exception_ptr) {
			std::rethrow_exception(_handle.promise().exception_ptr);
		}
		return _handle.promise().value;
	}

	std::future<T> get() {
		return _handle.promise().promise.get_future();
	}

	std::coroutine_handle<promise_type> _handle;

};


template<>
class Task<void> {
public:

	Task(const Task&) = delete;
	Task& operator=(const Task&) = delete;
	
	Task(Task&& other) noexcept : _handle(std::exchange(other._handle, nullptr)) {}
	Task& operator=(Task&& other) noexcept {
		if (this != &other) {
			if (_handle) {
				_handle.destroy();
			}
			_handle = std::exchange(other._handle, nullptr);
		}
		return *this;
	}

	~Task()
	{
		if (_handle) _handle.destroy();
	}

	struct promise_type {
		std::exception_ptr exception_ptr;
		std::coroutine_handle<> continuation;
		std::promise<void> promise;

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

		void return_void() { promise.set_value(); }
		void unhandled_exception() { 
			exception_ptr = std::current_exception();
			promise.set_exception(exception_ptr);
		}
	};


	Task(std::coroutine_handle<promise_type> h) : _handle(h) {}


	bool await_ready() { return !_handle || _handle.done(); }
	void await_suspend(std::coroutine_handle<> continuation_) {
		_handle.promise().continuation = continuation_;
		_handle.resume();
	}
	void await_resume() {
		if (_handle.promise().exception_ptr) {
			std::rethrow_exception(_handle.promise().exception_ptr);
		}
	}


	std::future<void> get() {
		return _handle.promise().promise.get_future();
	}

	std::coroutine_handle<promise_type> _handle;

};




}
}
#endif
