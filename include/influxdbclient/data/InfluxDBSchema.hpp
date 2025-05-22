namespace InfluxDB
{
namespace data
{

class InfluxDBSchema 
{
	virtual bool verify(InfluxDBPoint point) = 0;
}; 

}
}
