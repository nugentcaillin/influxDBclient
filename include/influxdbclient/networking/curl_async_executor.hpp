#include <curl/curl.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

namespace influxdbclient
{
namespace networking
{

class CurlAsyncExecutor
{
private:
	bool _running = false;
	std::mutex _mutex;
	std::thread _io_thread;
	CURLM *_multi_handle;
	std::condition_variable _action_cv;
	std::queue<CURL *> _handle_queue;

	CurlAsyncExecutor();
	~CurlAsyncExecutor();
	void run();

public:
	static CurlAsyncExecutor& getInstance();	
	void submit_handle(CURL *handle);
};


}
}
