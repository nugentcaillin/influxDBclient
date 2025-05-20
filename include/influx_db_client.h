#ifndef INFLUX_DB_H
#define INFLUX_DB_H



#include <vector>
#include <map>
#include <string>

namespace InfluxDBClient
{

class InfluxDBPoint;
class InfluxDBSchema;
template<class InfluxDBSchema>
class InfluxDBClient;


template<class InfluxDBSchema>
class InfluxDBClient 
{
private:
	std::string _token;
	std::string _org;
	std::string _bucket;
	std::string _precision;
	int _batch_size;
	std::vector<InfluxDBPoint> _batch;

public:
	
	InfluxDBClient(std::string token, std::string org, std::string bucket, std::string precision="s", int batch_size=1) {
		_token = token;
		_org = org;
		_bucket = bucket;
		_precision = precision;
		_batch_size = batch_size;
		_batch = std::vector<InfluxDBPoint>();
	}

	
};

class InfluxDBPoint 
{
private:
	std::map<std::string, std::string> _tags;
	std::map<std::string, std::string> _stringFields;
	std::map<std::string, float> _floatFields;
	std::map<std::string, int> _intFields;
	std::map<std::string, unsigned int> _unsignedIntFields;
	int _timestamp;	
public:

	std::string toLineProtocol(); 
};



class InfluxDBSchema 
{
	virtual bool verify(InfluxDBPoint point) = 0;
}; 

}

#endif
