#ifndef _FUTURE_AWAITABLE_H_
#define _FUTURE_AWAITABLE_H_

#include <future>
#include <coroutine>
#include <chrono>
#include <thread>
#include <utility>

namespace std
{

template <typename T>
struct _FutureAwaiter {
    std::future<T> future_; 

    _FutureAwaiter(std::future<T>&& f) : future_(std::move(f)) {}

    bool await_ready() const noexcept {
        return future_.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
    }

    void await_suspend(std::coroutine_handle<> awaiting_coroutine) {

        std::thread([this, awaiting_coroutine]() mutable {
            this->future_.wait(); 
            awaiting_coroutine.resume(); 
        }).detach(); 
    }

    T await_resume() {
        return future_.get(); 
    }
};

template <typename T>
auto operator co_await(std::future<T>&& future) noexcept {
    return _FutureAwaiter<T>{std::move(future)};
}




} // namespace std;


#endif // _FUTURE_AWAITABLE_H_
