cmake_minimum_required(VERSION 3.17)
project(socket_connector)

set(CMAKE_CXX_STANDARD 20)

include_directories(../)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

add_executable(socket_connector
        main.cpp
        connector.h
        console_connector.h
        platform_dep.h
        socket_connector.h
        ../Solver.cpp
        ../Solver.h
        ../Reader.cpp
        ../Reader.h)


target_link_libraries(${CMAKE_PROJECT_NAME} ws2_32)
