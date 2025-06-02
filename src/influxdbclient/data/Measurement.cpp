
#include "influxdbclient/data/measurement.hpp"

#include <utility>
#include <vector>
#include <string>

namespace influxdbclient
{
namespace data
{


std::string 
Measurement::escapeTagKeyValueFieldKey
( std::string str) const
{
	std::string escaped;
	for (char c : str)
	{
		if (c == ',' || c == '=' || c == ' ') escaped += '\\';
		escaped += c;
	}
	return escaped;
}
std::string 
Measurement::escapeFieldValue
( std::string str) const
{
	std::string escaped;
	for (char c : str)
	{
		if (c == '"' || c == '\\') escaped += '\\';
		escaped += c;
	}
	return escaped;
}
std::string
Measurement::escapeTable
( std::string str) const
{
	std::string escaped;
	for (char c : str)
	{
		if (c == ',' || c == ' ') escaped += '\\';
		escaped += c;
	}
	return escaped;
}


void 
Measurement::addTag
( const std::string& key
, const std::string& val)
{
	_tags.emplace_back(key, val);
}

void 
Measurement::addField
( const std::string& key
, const std::string& val)
{
	_fields.emplace_back(key, val);
}

void 
Measurement::setTimestamp
( long timestamp)
{
	_timestamp = timestamp;
}

void 
Measurement::setName
( const std::string& name)
{
	_name = name;
}

std::string
Measurement::getLineProtocol
() const
{
	std::string out = escapeTable(_name);
	for(auto& [key, val]: _tags)
	{
		out += ",";
		out += escapeTagKeyValueFieldKey(key);
		out += "=";
		out += escapeTagKeyValueFieldKey(val);
	}
	out += " ";
	for (auto it = _fields.begin(); it != _fields.end(); it++)
	{
		if (it != _fields.begin()) out += ",";
		out += escapeTagKeyValueFieldKey(it->first);
		out += "=";
		out += escapeFieldValue(it->second);
	}
	out += " ";
	out += std::to_string(_timestamp);
	out += "\n";
	return out;
}



} // namespace data
} // namespace influxdbclient

