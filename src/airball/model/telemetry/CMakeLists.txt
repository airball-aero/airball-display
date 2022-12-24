cmake_minimum_required(VERSION 3.5)
project(telemetry)

set(CMAKE_CXX_STANDARD 14)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(Boost REQUIRED program_options iostreams system)
include_directories(${BOOST_INCLUDE_DIRS})
message("boost lib: ${Boost_LIBRARIES}")
message("boost inc: ${Boost_INCLUDE_DIR}")

add_library(airball_telemetry_client
        telemetry.cpp
        udp_telemetry_client.cpp
        udp_packet_reader.cpp)
target_link_libraries(airball_telemetry_client
        Threads::Threads boost_system)
