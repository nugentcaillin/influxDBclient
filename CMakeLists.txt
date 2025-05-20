cmake_minimum_required(VERSION 3.1.0)

project(influx-db-client)



add_library(influx-db-client 
	src/influx-db-client.cpp
)

include(GNUInstallDirs)

set_target_properties(influx-db-client
	PROPERTIES
	CMAKE_CXX_STANDARD 20
	CMAKE_CXX_STANDARD_REQUIRED ON
)

target_include_directories(influx-db-client
	PUBLIC
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
	"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

install(TARGETS influx-db-client
	EXPORT influx-db-clientTargets
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES include/influx-db-client.h DESTINATION include)

install(EXPORT influx-db-clientTargets
	FILE influx-db-client-targets.cmake
	NAMESPACE influx-db-client::
	DESTINATION share/influx-db-client
)

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/influx-db-client-config.cmake"
	"include(CMakeFindDependencyMacro)\n"
	"include(\"\${CMAKE_CURRENT_LIST_DIR}/influx-db-client-targets.cmake\")\n"
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/influx-db-client.cmake-config"
	DESTINATION share/influx-db-client
)
