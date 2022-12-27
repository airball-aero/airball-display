add_subdirectory(telemetry)

add_library(model
        aerodynamics.cpp
        Airdata.cpp
        Settings.cpp)
target_link_libraries(model
        telemetry
        util)
