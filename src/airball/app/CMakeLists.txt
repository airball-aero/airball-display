cmake_minimum_required(VERSION 3.2)

add_executable(ab
        ab_main.cpp)
target_link_libraries(ab
        gflags::gflags airball_screen_linux)