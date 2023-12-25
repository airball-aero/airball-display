add_library(util
        LinearRateFilter.cpp
        file_write_watch.cpp
        one_shot_timer.cpp
        string_compression.cpp
        atomic_store.cpp
        Base16Encoding.cpp
        FileAdapter.cpp)
target_link_libraries(util
        z)

add_executable(file_write_watch_test
        file_write_watch_test_main.cpp)
target_link_libraries(file_write_watch_test
        util)

add_executable(linear_rate_filter_test
        linear_rate_filter_test_main.cpp)
target_link_libraries(linear_rate_filter_test
        util)

add_executable(one_shot_timer_test
        one_shot_timer_test.cpp)
target_link_libraries(one_shot_timer_test
        util)

add_executable(string_compression_test
        string_compression_test.cpp)
target_link_libraries(string_compression_test
        util)

add_executable(atomic_store_test
        atomic_store_test_main.cpp)
target_link_libraries(atomic_store_test
        util)

add_executable(Base16EncodingTest
        Base16EncodingTest_main.cpp)
target_link_libraries(Base16EncodingTest
        util)
