add_library(sound_scheme
        sound_scheme.cpp
        flyonspeed_scheme.cpp
        stallfence_scheme.cpp)
target_link_libraries(sound_scheme
        sound_mixer)

add_executable(sound_scheme_test
        sound_scheme_test_main.cpp)
target_link_libraries(sound_scheme_test
        sound_scheme)
