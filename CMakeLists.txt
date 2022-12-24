cmake_minimum_required(VERSION 3.2)

project(AirballDisplay)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#/ add_subdirectory(external/rapidjson)
# include_directories(external/rapidjson/include)
# TODO: Googletest CMake developed issues
# add_subdirectory(external/googletest/googletest)

add_subdirectory(gflags)
include_directories(gflags/src)

add_subdirectory(src/airball/app)
add_subdirectory(src/framework)
add_subdirectory(src/airball/model/telemetry)
add_subdirectory(src/airball/screen)

