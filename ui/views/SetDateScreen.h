#pragma once
#include "IView.h"
#include "TimetableLoader.h"
#include "lvgl/lv_types.h"

class SetDateScreen : public IView {
  TimetableLoader &loader;
  lv_obj_t *screen;
  lv_obj_t *day_roller;
  lv_obj_t *month_roller;
  lv_obj_t *year_roller;

  static void on_next_clicked(lv_event_t *e);
  static void on_month_or_year_changed(lv_event_t *e);
  static void update_days_roller(SetDateScreen* view);
  static int get_days_in_month(int year, int month);

public:
  SetDateScreen(TimetableLoader &loader);

  void create() override;
  lv_obj_t *getScreen() override;

private:
  static void delete_event_handler(lv_event_t *e);
};
