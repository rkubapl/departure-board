#define SDL_MAIN_HANDLED
#include "FileDataSource.h"
#include "TimetableLoader.h"
#include "lvgl/lvgl.h"
#include "ui.h"
#include <iostream>

static lv_display_t *display;
static lv_indev_t *mouse;
static lv_indev_t *mouse_wheel;
static lv_indev_t *keyboard;

#define WINDOW_WIDTH 320
#define WINDOW_HEIGHT 240

int main() {
  lv_init();

  display = lv_sdl_window_create(WINDOW_WIDTH, WINDOW_HEIGHT);
  mouse = lv_sdl_mouse_create();
  mouse_wheel = lv_sdl_mousewheel_create();
  keyboard = lv_sdl_keyboard_create();

  FileDataSource fileSource("schedule.bin");
  TimetableLoader loader(fileSource);

  try {
    loader.load();
  } catch (const std::exception &e) {
    std::cerr << "Blad podczas ladowania rozkladu: " << e.what() << std::endl;
  }

  ui_init(loader);

  while (1) {
    lv_timer_handler();
    lv_delay_ms(5);
  }

  return 0;
}