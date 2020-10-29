cmake_minimum_required(VERSION 3.17)
project(ericsson-socket_connector)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)
include_directories(/socket-connector)

add_executable(ericsson-socket_connector
        socket-connector/main.cpp
        socket-connector/connector.h
        socket-connector/console_connector.h
        socket-connector/platform_dep.h
        socket-connector/socket_connector.h
        Solver.cpp
        Solver.h
        Reader.cpp
        Reader.h)


target_link_libraries(${CMAKE_PROJECT_NAME} ws2_32)
