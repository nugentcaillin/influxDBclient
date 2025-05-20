cmake_minimum_required(VERSION 3.1.0)

project(InfluxDBClient)



add_library(InfluxDBClient 
	src/InfluxDBClient.cpp
)

include(GNUInstallDirs)

set_target_properties(InfluxDBClient
	PROPERTIES
	CMAKE_CXX_STANDARD 20
	CMAKE_CXX_STANDARD_REQUIRED ON
)

target_include_directories(InfluxDBClient
	PUBLIC
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
	"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

install(TARGETS InfluxDBClient
	EXPORT InfluxDBClientTargets
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES include/InfluxDBClient.h DESTINATION include)

install(EXPORT InfluxDBClientTargets
	FILE InfluxDBClientTargets.cmake
	NAMESPACE InfluxDBClient::
	DESTINATION share/InfluxDBClient
)

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/InfluxDBClient.cmake"
	"include(CMakeFindDependencyMacro)\n"
	"include(\"\${CMAKE_CURRENT_LIST_DIR}/InfluxDBClient.cmake\")\n"
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/InfluxDBClient.cmake"
	DESTINATION share/InfluxDBClient
)
