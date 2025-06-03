
#include <influxdbclient/influxdbclient.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <curl/curl.h>
#include <influxdbclient/networking/task.hpp>
#include <thread>
#include <chrono>
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>



int main()
{
	long writes = 1000000;
	long timestamp = 1748782999;
	std::vector<influxdbclient::data::Measurement> measurements;
	for (int i = 0; i < writes; i++)
	{
		auto m = influxdbclient::data::Measurement();
		m.setName("daily_price");
		m.addTag("ticker", "DEF");
		m.addTag("exchange", "ASX");
		m.addField("price", "28.99");
		m.addField("avg", "21.99");
		m.setTimestamp(timestamp);
		measurements.emplace_back(m);
		timestamp++;
	}

	influxdbclient::client::InfluxDBClient influxClient("http://localhost:8181", "apiv3_qbJ-y-y6FIn7oruLlXIO2aUlJw7MNhS0fbO24PMfgT85OYRRgR1u57wv0EoR0mZkhegWtjN3cQ8KtEpqrhrLeg", 500);

	std::vector<std::future<void>> futures;
	futures.reserve(writes);

	auto start = std::chrono::high_resolution_clock::now();
	
	for (int i = 0; i < writes; i++)
	{
		futures.emplace_back(influxClient.writeMeasurement(measurements[i], "stress_test", influxdbclient::data::TimePrecision::SECONDS).take_future());
	}
	for (int i = 0; i < writes; i++)
	{
		futures[i].get();
	}


	auto end = std::chrono::high_resolution_clock::now();


	std::cout << writes << " writes: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;

	return 0;
}
