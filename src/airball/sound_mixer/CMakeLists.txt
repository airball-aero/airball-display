add_library(sound_mixer
        balance_layer.cpp
        pwm_layer.cpp
        sine_envelope_layer.cpp
        sine_layer.cpp
        sound_layer.cpp
        sound_mixer.cpp)
target_link_libraries(sound_mixer
        asound)

add_executable(pwm_layer_test
        pwm_layer_test_main.cpp)
target_link_libraries(pwm_layer_test
        sound_mixer)

add_executable(sound_mixer_test
        sound_mixer_test_main.cpp)
target_link_libraries(sound_mixer_test
        sound_mixer)