if(ESP_PLATFORM)
    idf_component_register(
        SRCS
            ui.cpp
            views/SelectStop.cpp
            views/DepartureList.cpp
            views/VehicleRoute.cpp
            views/ErrorView.cpp
            fonts/roboto_14.c
            fonts/roboto_light_14.c
            fonts/roboto_medium_14.c
            fonts/roboto_bold_14.c
        INCLUDE_DIRS "."
        REQUIRES lvgl dataparser
    )
else()
    add_library(ui STATIC ui.cpp ui.h
            views/SelectStop.cpp
            views/SelectStop.h
            views/DepartureList.cpp
            views/DepartureList.h
            views/VehicleRoute.cpp
            views/VehicleRoute.h
            views/ErrorView.cpp
            views/ErrorView.h
            fonts/roboto_14.c
            fonts/roboto_light_14.c
            fonts/roboto_medium_14.c
            fonts/roboto_bold_14.c
            views/SetTimeScreen.cpp
            views/SetTimeScreen.h)

    target_include_directories(ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(ui PUBLIC lvgl)
    target_link_libraries(ui PUBLIC dataparser)
endif()
