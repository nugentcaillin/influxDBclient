
cmake_minimum_required(VERSION 3.1.0)
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")


project(influx_db_client)

message(INFO " using toolchain ${CMAKE_TOOLCHAIN_FILE}")

include(GNUInstallDirs)


find_package(spdlog CONFIG REQUIRED)
find_package(CURL CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_library(${PROJECT_NAME} STATIC)

target_sources(${PROJECT_NAME}
	PRIVATE
		src/influxdbclient/client/InfluxDBClient.cpp
		src/influxdbclient/networking/curl_global_initializer.cpp
		src/influxdbclient/networking/CurlAsyncExecutor.cpp
		src/influxdbclient/networking/CurlAwaitable.cpp
		src/influxdbclient/networking/LibcurlHttpClient.cpp
		src/influxdbclient/networking/HttpRequest.cpp
)



set_target_properties(influx_db_client
	PROPERTIES
	CMAKE_CXX_STANDARD 20
	CMAKE_CXX_STANDARD_REQUIRED ON
)

target_compile_options(influx_db_client PUBLIC -fcoroutines)


target_link_libraries(influx_db_client 
	PUBLIC 
		spdlog::spdlog
		CURL::libcurl
	PRIVATE
		nlohmann_json::nlohmann_json
)

target_include_directories(${PROJECT_NAME}
	PUBLIC
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
	"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/influxdbclient/networking>"
	"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/influxdbclient/networking_internals>"

		
)






install(TARGETS influx_db_client
	EXPORT influx_db_client_targets
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/influxdbclient" 
	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/influxdbclient")

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/influxdbclient/networking"
	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/influxdbclient/networking_internals"
	FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")


install(EXPORT influx_db_client_targets
	FILE influx_db_client-targets.cmake
	NAMESPACE influx_db_client::
	DESTINATION share/influx_db_client
)

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/influx_db_client-config.cmake"
	"include(CMakeFindDependencyMacro)\n"
	"include(\"\${CMAKE_CURRENT_LIST_DIR}/influx_db_client-targets.cmake\")\n"
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/influx_db_client-config.cmake"
	DESTINATION share/influx_db_client
)



