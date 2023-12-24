add_library(telemetry
        UdpTelemetry.cpp
        FakeTelemetry.cpp
        UdpPacketReader.cpp
        UdpPacketSender.cpp
        MessageBlockFormat.cpp
        SerialTelemetry.cpp)
target_link_libraries(telemetry
        Threads::Threads
        boost_system
        util)

add_executable(udp_packet_test
        udp_packet_test_main.cpp)
target_link_libraries(udp_packet_test
        telemetry)

add_executable(SerialTelemetryTest
        SerialTelemetryTest_main.cpp)
target_link_libraries(SerialTelemetryTest
        telemetry)
