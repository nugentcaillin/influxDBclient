namespace InfluxDB
{
namespace data
{
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

}
}
