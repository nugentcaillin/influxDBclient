#ifndef _MEASUREMENT_H_
#define _MEASUREMENT_H_

#include <utility>
#include <vector>
#include <string>

namespace influxdbclient
{
namespace data
{

class Measurement {
private:
	std::string _name;
	long _timestamp;
	std::vector<std::pair<std::string, std::string>> _tags;
	
	std::vector<std::pair<std::string, std::string>> _fields;
	
	std::string escapeTagKeyValueFieldKey(std::string str) const;
	std::string escapeFieldValue(std::string str) const;
	std::string escapeTable(std::string str) const;
public:
	void addTag(const std::string& key, const std::string& val);
	void addField(const std::string& key, const std::string& val);
	void setTimestamp(long timestamp);
	void setName(const std::string& name);



	std::string getLineProtocol() const;
};

} // namespace data
} // namespace influxdbclient

#endif // _MEASUREMENT_H_
