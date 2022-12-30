add_library(view
        AirballView.cpp)

add_library(widgets
        widgets.cpp)

target_link_libraries(view
        cairo widgets)