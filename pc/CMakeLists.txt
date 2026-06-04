cmake_minimum_required(VERSION 3.14)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(DepartureDisplay)

find_package(SDL2 REQUIRED)

include_directories(
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_INCLUDE_DIRS}/../
)

set(CONFIG_LV_BUILD_DEMOS OFF)
set(CONFIG_LV_BUILD_EXAMPLES OFF)
set(CONFIG_LV_USE_THORVG_INTERNAL OFF)

add_subdirectory(../lvgl ${CMAKE_CURRENT_BINARY_DIR}/lvgl)
add_subdirectory(../dataparser ${CMAKE_CURRENT_BINARY_DIR}/dataparser)
add_subdirectory(../ui ${CMAKE_CURRENT_BINARY_DIR}/ui)

set(SOURCES
        main.cpp
        main.h
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