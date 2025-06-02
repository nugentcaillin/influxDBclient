#ifndef _WRITE_BUFFER_H_
#define _WRITE_BUFFER_H_

#include "influxdbclient/data/measurement.hpp"

#include <vector>
#include <string>


namespace influxdbclient
{
namespace data
{

enum class TimePrecision
{
	MILLISECONDS,
	SECONDS,
	MICROSECONDS,
	NANOSECONDS
};

class WriteBuffer
{
private:
	std::string _name;
	TimePrecision _precision;
	std::string _lineProtocolMeasurements;
	size_t _capacity;
	size_t _curr;

public:
	WriteBuffer(const std::string& name, TimePrecision Precision, size_t capacity);
	void addMeasurement(const Measurement& measurement);
	bool isFull() const;
	const std::string& getName() const;
	TimePrecision getPrecision() const;
	std::string drainMeasurements();
};


} // namespace data 
} // namespace influxdbclient 

#endif //_WRITE_BUFFER_H_
