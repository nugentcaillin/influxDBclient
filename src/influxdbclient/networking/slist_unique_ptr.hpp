#ifndef _SLIST_UNIQUE_PTR_H_
#define _SLIST_UNIQUE_PTR_H_

#include <curl/curl.h>
#include <memory>


namespace influxdbclient
{
namespace networking
{

struct CurlSlistDeleter {
	void operator()(curl_slist *slist)
	{
		if (slist) curl_slist_free_all(slist);
	}
};

using UniqueCurlSlist = std::unique_ptr<curl_slist, CurlSlistDeleter>;

} // namespace networking
} // namespace influxdbclient

#endif // _SLIST_UNIQUE_PTR_H_
