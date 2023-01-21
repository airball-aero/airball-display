cmake_minimum_required(VERSION 3.2)

add_executable(ab
        ab_main.cpp)
target_link_libraries(ab
        gflags::gflags
        screen_linux
        util
        model
        view
        sound_mixer
        sound_scheme)

if (AIRBALL_BCM2835)

  target_link_libraries(ab
          screen_bcm2835)
  add_compile_definitions(AIRBALL_BCM2835=1)

endif()
