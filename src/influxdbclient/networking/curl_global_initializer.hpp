#ifndef _CURL_GLOBAL_INITIALIZER_H_
#define _CURL_GLOBAL_INITIALIZER_H_

#include <mutex>
#include <spdlog/spdlog.h>
#include <memory>

namespace influxdbclient
{
namespace networking
{

// singleton to ensure curl_global_init and curl_global_cleanup are each
// called exactly once
class CurlGlobalInitializer
{
private:
	static int _ref_count;
	static std::mutex _mutex;
	static std::shared_ptr<spdlog::logger> _logger; 
public:
	CurlGlobalInitializer();
	~CurlGlobalInitializer();
	
	// disable copy, assignment
	CurlGlobalInitializer(const CurlGlobalInitializer&) = delete;
	CurlGlobalInitializer& operator=(const CurlGlobalInitializer&) = delete;

};

}
}

#endif
