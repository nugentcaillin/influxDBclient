#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_

#include <string>
#include <map>




namespace influxdbclient
{
namespace networking
{


enum class HttpMethod {
	GET,
	POST
};


class HttpRequest {
private:
	std::string _url;
	std::string _body;
	std::map<std::string, std::string> _headers;
	HttpMethod _method;
public:
	
	const std::string& getUrl() const { return _url; }
	const std::string& getBody() const { return _body; }
	const std::map<std::string, std::string>& getHeaders() const { return _headers; }
	HttpMethod getMethod() const { return _method; }

	void setUrl(std::string url);
	void setBody(std::string body);
	void addHeader(std::string key, std::string value);
	void setMethod(HttpMethod method);

};


} // namespace networking	
} // namespace influxdbclient


#endif // _HTTP_REQUEST_H_

