#ifndef INFLUX_DB_H
#define INFLUX_DB_H



#include <vector>
#include <map>
#include <string>

namespace InfluxDB
{


namespace client
{

class InfluxDBClient 
{
private:
	std::string _token;
	std::string _org;
	int _batch_size;

public:
	
	// user provided splogger
	InfluxDBClient
	( std::string& token
	, std::string org
	, int batch_size=1

	) {
		_token = token;
		_org = org;
		_batch_size = batch_size;
	}

	
};

}
}

#endif
