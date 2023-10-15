add_library(telemetry
        NMEAFormat.cpp
        UdpTelemetry.cpp
        FakeTelemetry.cpp
        UdpPacketReader.cpp
        UdpPacketSender.cpp)
target_link_libraries(telemetry
        Threads::Threads
        boost_system)

add_executable(udp_packet_test
        udp_packet_test_main.cpp)
target_link_libraries(udp_packet_test
        telemetry)
