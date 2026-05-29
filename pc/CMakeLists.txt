cmake_minimum_required(VERSION 3.14)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
project(DepartureDisplay)

find_package(SDL2 REQUIRED)

include_directories(
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_INCLUDE_DIRS}/../
)

set(CONFIG_LV_BUILD_DEMOS OFF) # for demos
set(CONFIG_LV_BUILD_EXAMPLES OFF) # for examples
set(CONFIG_LV_USE_THORVG_INTERNAL OFF) # for ThorVG

add_subdirectory(../lvgl ${CMAKE_CURRENT_BINARY_DIR}/lvgl)
add_subdirectory(../dataparser ${CMAKE_CURRENT_BINARY_DIR}/dataparser)
add_subdirectory(../ui ${CMAKE_CURRENT_BINARY_DIR}/ui)

set(SOURCES main.cpp main.h roboto_14.c roboto_bold_14.c roboto_light_14.c question_mark.c
        FileDataSource.h
        FileDataSource.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${SDL2_INCLUDE_DIRS}
)

target_compile_options(${PROJECT_NAME} PRIVATE ${SDL2_CFLAGS})

target_link_libraries(${PROJECT_NAME} PRIVATE
        lvgl
        ${SDL2_LIBRARIES}
        ui
        dataparser
)