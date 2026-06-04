set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(ESP_PLATFORM)
    idf_component_register(
        SRCS
            TimetableLoader.cpp
            Departure.cpp
        INCLUDE_DIRS "."
    )
else()
    add_library(dataparser SHARED IDataSource.h TimetableLoader.cpp TimetableLoader.h Departure.cpp Departure.h ../esp32/main/MemoryDataSource.h ../esp32/main/MemoryDataSource.cpp)
    target_include_directories(dataparser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
endif()
