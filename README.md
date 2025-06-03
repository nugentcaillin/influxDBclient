# influxDBclient (v3)
A Modern, coroutine-based C++ Client library, built for high performance asyncronous workflows.

## Key Features
- implements core InfluxDB v3 API functions such as write, querying, creating and listing databases
- Batch writing capabilities for improved performance
- throughput of up to 400,000 writes per second for measurements
- IO multiplexing using libcurl CURLM interface
- Native C++20 coroutines for asynchronous execution
- promise/futures to bridge synchronous and asynchronous contexts
## Installation

- This project uses vcpkg to manage its external dependencies to ensure clean, reproducible builds
### Build
```console
git clone --recurse-submodules git@github.com:nugentcaillin/influxDBclient.git
cd build
cmake ..
```
### Linking in your CMake project
```console
add_subdirectory(influxDBclient)
target_link_libraries(main PRIVATE influx_db_client)
```
##### Note: if you're not using vcpkg globally, make sure the following dependencies are discoverable by CMake
```console
find_package(spdlog CONFIG REQUIRED)
find_package(CURL CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
```
> [!NOTE]
> CMake can't propogate vcpkg dependencies from a subdirectory, so you'll need to install them manually

## Usage

### Creating instance

```InfluxDbClient``` requires:
- ```url``` - e.g. "http://localhost:8181"
- ```token``` - InfluxDB auth token
- optional ```batch_size```, ```logger```, ```http_client```

Any combination of batch size, logger and http_client may be missing from constructor. defaults:
- ```batch_size```: 500
- ```logger``` - "influx_db_client_global_logger" if exists, null logger otherwise
- ```http_client``` - ```LibcurlHttpClient```

> [!TIP]
> If you're using multiple instances of InfluxDbClient it is reccommended they share a LibcurlHttpClient as each new LibcurlHttpClient spawns a new thread.

example:
```console
#include <influxdbclient/influxdbclient.hpp>
#include <spdlog/sinks/stdout_color_sinks.h>

int main()
{
    auto global = spdlog::stdout_color_mt("influx_db_client_global_logger");
    influxdbclient::client::InfluxDBClient influxClient(<database url>, <token>, 2);

    return 0;
}
```
output:
```console
[2025-06-03 16:35:47.584] [influx_db_client_global_logger] [info] Health check OK
[2025-06-03 16:35:47.630] [influx_db_client_global_logger] [info] Influx db client initialised with batch size of 2
```

### Getting values
InfluxDBClient returns values through a task wrapper ```influxdbclient::networking::Task<>```.
This wrapper can be co_awaited in coroutines, or the thread can be blocked until it is finished
by calling it's get() method, which waits for an internal future.

example:
```
// co awaiting inside a coroutine

influxdbclient::networking::Task<void> myCoroutine(influxdbclient::client::InfluxDBClient& client)
{
    // some logic 
    auto result = co_await client.querySql(database, someQuery);

    // do something with result
    co_return;
}

// waiting syncronously

auto result = client.querySql(database, someQuery).get()

```
> [!WARNING]
> the get() method resumes the handle of the coroutine, only use this to wait syncronously
> for a coroutine. If that coroutine has made its way past its initial suspend, calling get() is undefined behaviour

> [!WARNING]
> get() blocks the thread it is called on until this coroutine is finished, use it only when you do not want a coroutine to 
> be performed asyncronously

> [!NOTE]
> examples in this readme are primarily using the get() method. This is primarily for simplicity, it is usually better to co_await them.


### Listing databases

```influxdbclient::networking::Task<std::vector<std::string>> InfluxDbClient::listDatabases()```
Returns a vector of string database names
example:
```
std::vector<std::string> dbs = influxClient.listDatabases().get();
for (auto& db: dbs) {
	std::cout << db << std::endl;
}
```
output:
```
[2025-06-03 23:01:24.865] [influx_db_client_global_logger] [info] list database
_internal
test
test4
test8
```

### Creating databases
```influxdbclient::networking::Task<void> InfluxDBClient::createDatabase(const std:: string& name)```
creates a database.

### Writing measurements
Writing to the database is done using ```influxdbclient::data::Measurement``` and ```influxdbclient::networking::Task<void> InfluxDBClient::writeMeasurement(const influxdbclient::data::Measurement& measurement, const std:string& name, influxdbclient::data::TimePrecision precision)```

measurements are constructed using Measurement's ```setName```, ```addTag```, ```addField``` and ```setTimestamp```.
```setName``` takes a const std::string& as an argument
```addTag``` and ```addField``` each take two const std::string& representing key and value
```setTimestamp``` takes a long

Writes are batched, but if you want to force a write you can use ```influxdbclient::networking::Task<void> flushWriteBuffer(const std::string& name, influxdbclient::data::TimePrecision precision)```

example
```

auto m = influxdbclient::data::Measurement();
m.setName("daily_price");
m.addTag("ticker", "GHI");
m.addTag("exchange", "ASX");
m.addField("price", "23.99");
m.addField("avg", "28.99");
m.setTimestamp(1748781999);
	

auto m2 = influxdbclient::data::Measurement();
m2.setName("daily_price");
m2.addTag("ticker", "GHI");
m2.addTag("exchange", "ASX");
m2.addField("price", "23.99");
m2.addField("avg", "34.99");
m2.setTimestamp(1748782999);
	

influxClient.writeMeasurement(std::move(m), "test8", influxdbclient::data::TimePrecision::SECONDS).get();
influxClient.writeMeasurement(std::move(m2), "test8", influxdbclient::data::TimePrecision::SECONDS).get();
influxClient.flushWriteBuffer("test8", influxdbclient::data::TimePrecision::SECONDS).get()
	

```
output
```
[2025-06-03 23:24:05.638] [influx_db_client_global_logger] [info] Written 2 measurements to test8 with precision: second
```

### Querying measurements
Currently only sql queries are supported, and are done using ```influxdbclient::networking::Task<nlohmann::json> querySql( const std::string& database, const std::string& query, const std::map<std::string, std::string> params);```.
This allows for both raw and parametrised queries

example
```
std::string name = "test8";
std::string query = "SELECT * FROM daily_price";
std::map<std::string, std::string> params;
auto json = influxClient.querySql(name, query, params).get();
std::cout << json.dump(4) << std::endl;
	
query = "SELECT * FROM daily_price WHERE daily_price.ticker=$ticker";
params.emplace("ticker", "ABC");
json = influxClient.querySql(name, query, params).get();
std::cout << json.dump(4) << std::endl;
```
output
```
[2025-06-03 23:50:54.752] [influx_db_client_global_logger] [info] queried database test8 returning 4 measurements
[
    {
        "avg": 34.99,
        "exchange": "ASX",
        "price": 23.99,
        "ticker": "ABC",
        "time": "2025-06-01T13:03:19"
    },
    {
        "avg": 34.99,
        "exchange": "ASX",
        "price": 23.99,
        "ticker": "GHI",
        "time": "2025-06-01T13:03:19"
    },
    {
        "avg": 21.99,
        "exchange": "ASX",
        "price": 28.99,
        "ticker": "ABC",
        "time": "2025-06-01T12:46:39"
    },
    {
        "avg": 28.99,
        "exchange": "ASX",
        "price": 23.99,
        "ticker": "GHI",
        "time": "2025-06-01T12:46:39"
    }
]
[2025-06-03 23:50:54.860] [influx_db_client_global_logger] [info] queried database test8 returning 2 measurements
[
    {
        "avg": 34.99,
        "exchange": "ASX",
        "price": 23.99,
        "ticker": "ABC",
        "time": "2025-06-01T13:03:19"
    },
    {
        "avg": 21.99,
        "exchange": "ASX",
        "price": 28.99,
        "ticker": "ABC",
        "time": "2025-06-01T12:46:39"
    }
]
```

## License


MIT License

Copyright (c) 2025 Caillin Nugent

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.


