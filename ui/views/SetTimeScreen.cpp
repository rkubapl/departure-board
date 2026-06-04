//
// Created by Jakub Rajchel on 6/1/26.
//

#include "SetTimeScreen.h"
#include "SelectStop.h"
#include "ui.h"
#include <ctime>
#include <string>
#include <sys/time.h>
#include <unistd.h>

static void calendar_event_cb(lv_event_t *e) {
  if (lv_event_get_code(e) != LV_EVENT_VALUE_CHANGED) return;

  lv_obj_t *calendar = (lv_obj_t *)lv_event_get_current_target(e);
  if (!calendar) return;

  lv_calendar_date_t date;
  if (lv_calendar_get_pressed_date(calendar, &date) == LV_RESULT_OK) {
    lv_calendar_set_today_date(calendar, date.year, date.month, date.day);
  }
}

SetTimeScreen::SetTimeScreen(uint64_t initialTime, TimetableLoader &loader)
    : initialTime(initialTime), loader(loader), screen(nullptr),
      calendar(nullptr), hour_roller(nullptr), min_roller(nullptr)
    {}

void SetTimeScreen::create() {
  screen = lv_obj_create(NULL);

  lv_obj_set_layout(screen, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_style_pad_all(screen, 10, LV_PART_MAIN);
  lv_obj_set_flex_align(screen, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);

  lv_obj_t *titleText = lv_label_create(screen);
  lv_label_set_text(titleText, "Ustaw datę i czas");
  lv_obj_set_style_text_font(titleText, &roboto_bold_14, 0);
  lv_obj_set_width(titleText, lv_pct(100));
  lv_obj_set_style_text_align(titleText, LV_TEXT_ALIGN_CENTER, 0);

  auto timeObj = static_cast<time_t>(initialTime);
  tm *timeInfo = localtime(&timeObj);

  calendar = lv_calendar_create(screen);
  lv_obj_add_event_cb(calendar, calendar_event_cb, LV_EVENT_VALUE_CHANGED,
                      NULL);
  lv_obj_set_size(calendar, 200, 200);

  if (timeInfo) {
    lv_calendar_set_today_date(calendar, timeInfo->tm_year + 1900,
                               timeInfo->tm_mon + 1, timeInfo->tm_mday);
    lv_calendar_set_showed_date(calendar, timeInfo->tm_year + 1900,
                                timeInfo->tm_mon + 1);
  }
  lv_calendar_header_arrow_create(calendar);

  static const char *day_names[7] = {"Pn", "Wt", "Śr", "Cz", "Pt", "Sb", "Nd"};
  lv_calendar_set_day_names(calendar, day_names);
  lv_obj_set_style_text_font(calendar, &roboto_14, 0);

  lv_obj_t *time_cont = lv_obj_create(screen);
  lv_obj_set_layout(time_cont, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(time_cont, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(time_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);
  lv_obj_set_size(time_cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  lv_obj_set_style_border_width(time_cont, 0, LV_PART_MAIN);
  lv_obj_set_style_bg_opa(time_cont, LV_OPA_TRANSP, LV_PART_MAIN);

  hour_roller = lv_roller_create(time_cont);
  lv_roller_set_options(hour_roller,
                        "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13"
                        "\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23",
                        LV_ROLLER_MODE_NORMAL);
  lv_obj_set_style_text_font(hour_roller, &roboto_14, 0);
  if (timeInfo)
    lv_roller_set_selected(hour_roller, timeInfo->tm_hour, LV_ANIM_OFF);
  lv_roller_set_visible_row_count(hour_roller, 3);

  lv_obj_t *colon = lv_label_create(time_cont);
  lv_label_set_text(colon, ":");
  lv_obj_set_style_text_font(colon, &roboto_bold_14, 0);

  min_roller = lv_roller_create(time_cont);
  std::string min_opts = "";
  for (int i = 0; i < 60; i++) {
    if (i < 10) min_opts += "0" + std::to_string(i);
    else min_opts += std::to_string(i);
    if (i != 59) min_opts += "\n";
  }

  lv_roller_set_options(min_roller, min_opts.c_str(), LV_ROLLER_MODE_NORMAL);
  lv_obj_set_style_text_font(min_roller, &roboto_14, 0);

  if (timeInfo)
    lv_roller_set_selected(min_roller, timeInfo->tm_min, LV_ANIM_OFF);

  lv_roller_set_visible_row_count(min_roller, 3);

  lv_obj_t *save_btn = lv_btn_create(screen);
  lv_obj_add_event_cb(save_btn, on_save_clicked, LV_EVENT_CLICKED, this);
  lv_obj_t *save_label = lv_label_create(save_btn);
  lv_label_set_text(save_label, "Zapisz");
  lv_obj_set_style_text_font(save_label, &roboto_14, 0);
}

void SetTimeScreen::on_save_clicked(lv_event_t *e) {
  SetTimeScreen *view = (SetTimeScreen *)lv_event_get_user_data(e);

  const lv_calendar_date_t *date = lv_calendar_get_today_date(view->calendar);
  uint32_t hour = lv_roller_get_selected(view->hour_roller);
  uint32_t minute = lv_roller_get_selected(view->min_roller);

  tm t = {};
  t.tm_year = date->year - 1900;
  t.tm_mon = date->month - 1;
  t.tm_mday = date->day;
  t.tm_hour = hour;
  t.tm_min = minute;
  t.tm_sec = 0;
  t.tm_isdst = 0;

  time_t new_time = mktime(&t);

  if (new_time != -1) {
    timeval tv = {};
    tv.tv_sec = new_time;
    tv.tv_usec = 0;
    settimeofday(&tv, NULL);
  }

  auto *stopView = new SelectStop(view->loader);
  stopView->create();
  lv_screen_load_anim(stopView->getScreen(), LV_SCREEN_LOAD_ANIM_OUT_LEFT, 0, 0,
                      false);
}

lv_obj_t *SetTimeScreen::getScreen() { return screen; }
