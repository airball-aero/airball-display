add_library(screen_linux
        framebuffer_screen.cpp
        image_screen.cpp
        x11_screen.cpp)
target_link_libraries(screen_linux
        X11 cairo)

if (AIRBALL_BCM2835)

    add_library(bcm2835_smi_ioctl_defs
            bcm2835_smi_ioctl_defs.cpp)
    target_include_directories(bcm2835_smi_ioctl_defs PRIVATE
           /usr/src/linux-headers-5.15.76+/include
           /usr/src/linux-headers-5.15.76-v7+/include
           /usr/src/linux-headers-5.15.76-v7l+/include)

    add_library(screen_bcm2835
            st7789vi_frame_writer.cpp
            st7789vi_frame_writer_gpio.cpp
            st7789vi_frame_writer_smi.cpp
            st7789vi_screen.cpp)
    target_link_libraries(screen_bcm2835
            bcm2835_smi_ioctl_defs
            pigpio)

    add_executable(st7789vi_frame_writer_test
            st7789vi_frame_writer_test_main.cpp)
    target_link_libraries(st7789vi_frame_writer_test
            screen_bcm2835)

endif ()
