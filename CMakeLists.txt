cmake_minimum_required(VERSION 3.1.0)

project(influx_db_client)


include(GNUInstallDirs)

add_library(influx_db_client 
	src/influx_db_client.cpp
)



set_target_properties(influx_db_client
	PROPERTIES
	CMAKE_CXX_STANDARD 20
	CMAKE_CXX_STANDARD_REQUIRED ON
)

target_include_directories(influx_db_client
	PUBLIC
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
	"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

install(TARGETS influx_db_client
	EXPORT influx_db_client_targets
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES influx_db_client.h DESTINATION include)



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
