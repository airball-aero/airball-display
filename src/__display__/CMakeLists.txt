find_package(Boost REQUIRED program_options iostreams system)
include_directories(${BOOST_INCLUDE_DIRS})
message("boost lib: ${Boost_LIBRARIES}")
message("boost inc: ${Boost_INCLUDE_DIR}")

add_library(airball_display
        ../airball/view/widgets.cpp ../airball/model/Airdata.cpp ../airball/model/Settings.cpp ../airball/view/AirballView.cpp ../airball/util/one_shot_timer.cpp
        controller.cpp ../airball/screen/x11_screen.cpp user_input_source.cpp delay_timer.cpp
        ../airball/model/aerodynamics.cpp ../airball/model/telemetry/FakeTelemetry.cpp interpolation_table.cpp
        system_status.cpp device_keyboard_listener.cpp
        log_reader_telemetry_client.cpp ../airball/screen/image_screen.cpp ../airball/util/LinearRateFilter.cpp
        http_server.cpp ../airball/util/file_write_watch.cpp ../airball/sound_mixer/sound_mixer.cpp ../airball/sound_mixer/sound_layer.cpp
        ../airball/sound_mixer/sine_layer.cpp ../airball/sound_mixer/sine_envelope_layer.cpp ../airball/sound_mixer/pwm_layer.cpp ../airball/sound_mixer/balance_layer.cpp
        ../airball/sound_scheme/sound_scheme.cpp ../airball/sound_scheme/stallfence_scheme.cpp ../airball/sound_scheme/flyonspeed_scheme.cpp ../airball/util/polyfit.cpp
        ../airball/screen/st7789vi_frame_writer.cpp ../airball/screen/st7789vi_frame_writer_gpio.cpp
        ../airball/screen/st7789vi_frame_writer_smi.cpp ../airball/screen/st7789vi_frame_writer_test_main.cpp
        ../airball/screen/framebuffer_screen.cpp ../airball/screen/x11_screen.cpp ../airball/screen/st7789vi_screen.cpp ../airball/app/ab_main.cpp)
target_link_libraries(airball_display
        X11 cairo pthread asound pigpio
        airball_telemetry_client
        ${Boost_LIBRARIES})

add_library(bcm2835_smi_ioctl_defs
        ../airball/screen/bcm2835_smi_ioctl_defs.cpp)
target_include_directories(bcm2835_smi_ioctl_defs PRIVATE
        /usr/src/linux-headers-5.10.103-v7+/include)

add_executable(sound_mixer
        ../airball/sound_mixer/sound_mixer_test_main.cpp)
target_link_libraries(sound_mixer
        airball_display)

add_executable(sound_scheme
        ../airball/sound_scheme/sound_scheme_main.cpp)
target_link_libraries(sound_scheme
        airball_display)

add_executable(pwm_layer
        ../airball/sound_mixer/pwm_layer_test_main.cpp)
target_link_libraries(pwm_layer
        airball_display)

# add_executable(ab
#         ab_main.cpp ../app/ab_main.cpp)
# target_link_libraries(ab
#         airball_display bcm2835_smi_ioctl_defs)

add_executable(replay
        replay_main.cpp)
target_link_libraries(replay
        airball_display airball_telemetry_client)

add_executable(user_input_source_test
        user_input_source_test_main.cpp)
target_link_libraries(user_input_source_test
        airball_display)

add_executable(device_keyboard_listener_test
        device_keyboard_listener_test_main.cpp)
target_link_libraries(device_keyboard_listener_test
        airball_display)

add_executable(http_server_test
        http_server_test_main.cpp)
target_link_libraries(http_server_test
        airball_display)

add_executable(file_write_watch_test
        ../airball/util/file_write_watch_test_main.cpp)
target_link_libraries(file_write_watch_test
        airball_display)

add_executable(st7789vi_frame_writer_test
        ../airball/screen/st7789vi_frame_writer_test_main.cpp)
target_link_libraries(st7789vi_frame_writer_test
        airball_display bcm2835_smi_ioctl_defs)

add_executable(fb_fps_test
        ../airball/screen/fb_fps_test_main.cpp)

add_executable(display_tests
        aerodynamics_test.cpp ../airball/util/one_shot_timer_test.cpp delay_timer_test.cpp
        interpolation_table_test.cpp airdata_test.cpp
        ../airball/model/telemetry/fake_telemetry_client_test.cpp system_status_test.cpp)
target_link_libraries(display_tests
        gtest gtest_main airball_display)
