cmake_minimum_required(VERSION 3.17)
project(ericsson-socket_connector)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)
include_directories(/socket-connector)
include_directories(/win32-draw)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wno-sign-compare -Wuninitialized -O2")

#option(USE_WERROR "turn on -Werror flag" ON)
#if(USE_WERROR)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
#endif(USE_WERROR)

add_executable(ericsson-socket_connector
        socket-connector/main.cpp
        socket-connector/connector.h
        socket-connector/console_connector.h
        socket-connector/platform_dep.h
        socket-connector/socket_connector.h
        win32-draw/Draw.cpp
        win32-draw/Draw.hpp
        DisplayInfo.hpp
        Solver.cpp
        Solver.h
        Reader.cpp
        Reader.h
        Solver_finished_functions.cpp)


target_link_libraries(${CMAKE_PROJECT_NAME} ws2_32)
