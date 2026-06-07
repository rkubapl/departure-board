#pragma once
#include "IView.h"
#include "TimetableLoader.h"
#include "lvgl/lv_types.h"

class SetTimeScreen : public IView {
  TimetableLoader &loader;
  lv_obj_t *screen;
  lv_obj_t *hour_roller;
  lv_obj_t *min_roller;

  int selected_year;
  int selected_month;
  int selected_day;

  static void on_save_clicked(lv_event_t *e);
  static void on_back_clicked(lv_event_t *e);

public:
  SetTimeScreen(TimetableLoader &loader, int year, int month, int day);

  void create() override;
  lv_obj_t *getScreen() override;

private:
  static void delete_event_handler(lv_event_t *e);
};
