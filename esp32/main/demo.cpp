#include <math.h>
#include <stdio.h>

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#include <esp_check.h>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_system.h>

#include <esp_lvgl_port.h>
#include <lvgl.h>

extern "C" {
#include "driver/spi_master.h"
#include "esp_vfs_fat.h"
#include "lcd.h"
#include "sdmmc_cmd.h"
#include "touch.h"
}

#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/unistd.h>

#include "MemoryDataSource.h"
#include "../../ui/TimeManager.h"
#include "dataparser/TimetableLoader.h"
#include "ui/ui.h"

#define PIN_NUM_MISO (gpio_num_t) GPIO_NUM_19
#define PIN_NUM_MOSI (gpio_num_t) GPIO_NUM_23
#define PIN_NUM_CLK (gpio_num_t) GPIO_NUM_18
#define PIN_NUM_CS (gpio_num_t) GPIO_NUM_5

static const char *TAG = "demo";

static TimetableLoader *global_loader = nullptr;
static MemoryDataSource *global_mem_source = nullptr;

extern "C" void app_main(void) {
  esp_lcd_panel_io_handle_t lcd_io;
  esp_lcd_panel_handle_t lcd_panel;
  esp_lcd_touch_handle_t tp;
  lvgl_port_touch_cfg_t touch_cfg = {};
  lv_display_t *lvgl_display = NULL;

  // microSD Card
  esp_vfs_fat_sdmmc_mount_config_t mount_config = {};
  mount_config.format_if_mount_failed = false;
  mount_config.max_files = 1;
  mount_config.allocation_unit_size = 16 * 1024;
  sdmmc_card_t *card;
  const char mount_point[] = "/sdcard";
  ESP_LOGI(TAG, "Initializing SD card");

  sdmmc_host_t host = SDSPI_HOST_DEFAULT();
  spi_bus_config_t bus_cfg = {};
  bus_cfg.mosi_io_num = PIN_NUM_MOSI;
  bus_cfg.miso_io_num = PIN_NUM_MISO;
  bus_cfg.sclk_io_num = PIN_NUM_CLK;
  bus_cfg.quadwp_io_num = -1;
  bus_cfg.quadhd_io_num = -1;
  bus_cfg.max_transfer_sz = 4000;

  esp_err_t ret = spi_bus_initialize((spi_host_device_t)host.slot, &bus_cfg,
                                     SDSPI_DEFAULT_DMA);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to initialize bus.");
  } else {
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = PIN_NUM_CS;
    slot_config.host_id = (spi_host_device_t)host.slot;

    ESP_LOGI(TAG, "Mounting filesystem");
    ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config,
                                  &mount_config, &card);

    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "Failed to mount filesystem.");
    } else {
      ESP_LOGI(TAG, "Filesystem mounted");
      sdmmc_card_print_info(stdout, card);

      ESP_LOGI(TAG, "Opening /sdcard/schedule.bin");
      FILE *f = fopen("/sdcard/schedule.bin", "rb");
      if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open schedule.bin for reading");
      } else {
        fseek(f, 0, SEEK_END);
        long file_size = ftell(f);
        fseek(f, 0, SEEK_SET);
        ESP_LOGI(TAG, "schedule.bin opened successfully. Size: %ld bytes",
                 file_size);

        uint8_t *file_buf = (uint8_t *)malloc(file_size);
        if (file_buf) {
          size_t bytes_read = fread(file_buf, 1, file_size, f);
          ESP_LOGI(TAG, "Read %zu bytes (expected %ld)", bytes_read, file_size);

          global_mem_source = new MemoryDataSource(file_buf, file_size);
          global_loader = new TimetableLoader(*global_mem_source);

          try {
            global_loader->load();
            ESP_LOGI(TAG,
                     "Data loaded into memory and TimetableLoader initialized");

            struct timeval tv = {};
            tv.tv_sec = global_loader->getStartDate() * 86400 + 8 * 3600;
            TimeManager::setTime(tv.tv_sec);
            ESP_LOGI(TAG, "System time set to timetable start date (8:00 AM)");
          } catch (const std::exception &e) {
            ESP_LOGE(TAG, "Failed to load schedule: %s", e.what());
          }
        } else {
          ESP_LOGE(TAG, "Failed to allocate %ld bytes for schedule.bin",
                   file_size);
        }

        fclose(f);
      }
      esp_vfs_fat_sdcard_unmount(mount_point, card);
      ESP_LOGI(TAG, "Filesystem unmounted");
    }

    spi_bus_free((spi_host_device_t)host.slot);
    ESP_LOGI(TAG, "SPI bus freed");
  }

  ESP_ERROR_CHECK(lcd_display_brightness_init());

  ESP_ERROR_CHECK(app_lcd_init(&lcd_io, &lcd_panel));
  lvgl_display = app_lvgl_init(lcd_io, lcd_panel);
  if (lvgl_display == NULL) {
    ESP_LOGI(TAG, "Fatal error in app_lvgl_init");
    esp_restart();
  }

  ESP_ERROR_CHECK(touch_init(&tp));
  touch_cfg.disp = lvgl_display;
  touch_cfg.handle = tp;
  touch_cfg.scale.x = 0;
  touch_cfg.scale.y = 0;
  lvgl_port_add_touch(&touch_cfg);

  ESP_ERROR_CHECK(lcd_display_brightness_set(100));
  ESP_ERROR_CHECK(lcd_display_rotate(lvgl_display, LV_DISPLAY_ROTATION_90));

  if (global_loader) {
    ui_init(*global_loader);
  } else {
    ui_error_screen("Error", "Failed to load data from microSD card.");
  }

  while (1) {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
  vTaskDelay(portMAX_DELAY);
}
