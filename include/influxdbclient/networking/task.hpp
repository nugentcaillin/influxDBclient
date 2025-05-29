#ifndef _TASK_H_
#define _TASK_H_


#include <coroutine>
#include <exception>
#include <iostream>

namespace influxdbclient
{
namespace networking
{

template<typename T>
class Task {
public:
	struct promise_type {
		T value;
		std::exception_ptr exception_ptr;
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

		void return_value(T value_) { value = value_; }
		void unhandled_exception() { 
			exception_ptr = std::current_exception(); 
		}
	};


	Task(std::coroutine_handle<promise_type> h) : _handle(h) {}

	~Task() {
		if (_handle)
		{
			if (!_handle.done()) {
				_handle.resume();
			}
			_handle.destroy();
		}
	}

	bool await_ready() { return !_handle || _handle.done(); }
	void await_suspend(std::coroutine_handle<> continuation_) {
		_handle.promise().continuation = continuation_; 
	}
	T await_resume() {

		if (_handle.promise().exception_ptr) {
			std::rethrow_exception(_handle.promise().exception_ptr);
		}
		return _handle.promise().value;
	}

	T get() {
		if (_handle && !_handle.done()) _handle.resume();
		return await_resume();
	}

	std::coroutine_handle<promise_type> _handle;

};


template<>
class Task<void> {
public:
	struct promise_type {
		std::exception_ptr exception_ptr;
		std::coroutine_handle<> continuation;

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

		void return_void() {}
		void unhandled_exception() { exception_ptr = std::current_exception(); }
	};


	Task(std::coroutine_handle<promise_type> h) : _handle(h) {}

	~Task() {
		if (_handle)
		{
			if (!_handle.done()) {
				_handle.resume();
			}
			_handle.destroy();
		}
	}

	bool await_ready() { return !_handle || _handle.done(); }
	void await_suspend(std::coroutine_handle<> continuation_) {
		_handle.promise().continuation = continuation_; 
	}
	void await_resume() {

		if (_handle.promise().exception_ptr) {
			std::rethrow_exception(_handle.promise().exception_ptr);
		}
	}

	std::coroutine_handle<promise_type> _handle;

};




}
}
#endif
