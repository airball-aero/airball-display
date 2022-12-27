add_library(util
        cubic_rate_filter.cpp
        file_write_watch.cpp
        polyfit.cpp)

add_executable(file_write_watch_test
        file_write_watch_test_main.cpp)
target_link_libraries(file_write_watch_test
        util)
