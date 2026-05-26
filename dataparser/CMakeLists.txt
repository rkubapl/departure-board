add_library(dataparser SHARED IDataSource.cpp IDataSource.h TimetableLoader.cpp TimetableLoader.h Departure.cpp Departure.h)
target_include_directories(dataparser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})