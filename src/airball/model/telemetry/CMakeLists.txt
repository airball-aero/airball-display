add_library(telemetry
        ITelemetry.cpp
        UdpTelemetry.cpp
        FakeTelemetry.cpp
        UdpPacketReader.cpp)
target_link_libraries(telemetry
        Threads::Threads
        boost_system)

add_executable(udp_packet_reader
        udp_packet_reader_main.cpp)
target_link_libraries(udp_packet_reader
        telemetry)
