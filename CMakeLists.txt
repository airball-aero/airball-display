cmake_minimum_required(VERSION 3.2)

project(airball-display)

set(AIRBALL_BCM2835 1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(Boost REQUIRED program_options iostreams system)
include_directories(${BOOST_INCLUDE_DIRS})
message("boost lib: ${Boost_LIBRARIES}")
message("boost inc: ${Boost_INCLUDE_DIR}")

add_subdirectory(gflags)
include_directories(gflags/src)

add_subdirectory(rapidjson)
include_directories(rapidjson/include)

add_subdirectory(src)
