idf_component_register(SRCS demo.cpp lcd.c touch.c MemoryDataSource.cpp MemoryDataSource.h
                    INCLUDE_DIRS "."
                    REQUIRES esp_driver_ledc lvgl ui dataparser fatfs sdmmc
                    )
