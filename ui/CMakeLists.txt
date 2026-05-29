add_library(ui STATIC ui.cpp ui.h
        views/SelectStop.cpp
        views/SelectStop.h
        views/DepartureList.cpp
        views/DepartureList.h
        views/VehicleRoute.cpp
        views/VehicleRoute.h)

target_include_directories(ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ui PUBLIC lvgl)
target_link_libraries(ui PUBLIC dataparser)