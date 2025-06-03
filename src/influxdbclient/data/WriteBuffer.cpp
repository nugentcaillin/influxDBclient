#include "influxdbclient/data/measurement.hpp"
#include "influxdbclient/data/write_buffer.hpp"

#include <vector>
#include <iostream>
#include <string>




namespace influxdbclient
{
namespace data
{


WriteBuffer::WriteBuffer
( const std::string& name
, TimePrecision precision
, size_t capacity)
: _name(name)
, _precision(precision)
, _lineProtocolMeasurements()
, _capacity(capacity)
, _curr(0)
{
}

void 
WriteBuffer::addMeasurement
( const Measurement& measurement)
{
	_lineProtocolMeasurements += measurement.getLineProtocol();
	_curr++;
}

bool 
WriteBuffer::isFull
() const
{
	return _curr >= _capacity;
}

const std::string& 
WriteBuffer::getName
() const
{
	return _name;
}

TimePrecision 
WriteBuffer::getPrecision
() const
{
	return _precision;
}

std::pair<std::string, int>
WriteBuffer::drainMeasurements
()
{
	std::string drainedMeasurements;
	drainedMeasurements.swap(_lineProtocolMeasurements);
	auto pair = std::make_pair(drainedMeasurements, _curr);
	_curr = 0;
	return pair;
}


	



} // namespace data 
} // namespace influxdbclient 
