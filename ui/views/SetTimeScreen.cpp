#include "SetTimeScreen.h"
#include "../ScreenManager.h"
#include "../TimeManager.h"
#include "SelectStop.h"
#include "ui.h"
#include <ctime>
#include <string>

SetTimeScreen::SetTimeScreen(TimetableLoader &loader, int year, int month, int day)
    : loader(loader), screen(nullptr), hour_roller(nullptr),
      min_roller(nullptr), selected_year(year), selected_month(month), selected_day(day) {}

void SetTimeScreen::create() {
  screen = lv_obj_create(NULL);
  lv_obj_add_event_cb(screen, delete_event_handler, LV_EVENT_DELETE, this);
  lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_set_layout(screen, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_style_pad_all(screen, 10, LV_PART_MAIN);
  lv_obj_set_flex_align(screen, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

  lv_obj_t *titleText = lv_label_create(screen);
  lv_label_set_text(titleText, "Wybierz czas");
  lv_obj_set_style_text_font(titleText, &roboto_bold_14, 0);
  lv_obj_set_width(titleText, lv_pct(100));
  lv_obj_set_style_text_align(titleText, LV_TEXT_ALIGN_CENTER, 0);

  auto timeObj = TimeManager::getTime();
  tm *timeInfo = localtime(&timeObj);

  lv_obj_t *time_step_cont = lv_obj_create(screen);
  lv_obj_clear_flag(time_step_cont, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_width(time_step_cont, lv_pct(100));
  lv_obj_set_flex_grow(time_step_cont, 1);
  lv_obj_set_layout(time_step_cont, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(time_step_cont, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(time_step_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_border_width(time_step_cont, 0, LV_PART_MAIN);
  lv_obj_set_style_bg_opa(time_step_cont, LV_OPA_TRANSP, LV_PART_MAIN);

  lv_obj_t *time_cont = lv_obj_create(time_step_cont);
  lv_obj_clear_flag(time_cont, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_layout(time_cont, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(time_cont, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(time_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_size(time_cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  lv_obj_set_style_border_width(time_cont, 0, LV_PART_MAIN);
  lv_obj_set_style_bg_opa(time_cont, LV_OPA_TRANSP, LV_PART_MAIN);

  hour_roller = lv_roller_create(time_cont);
  lv_obj_set_style_text_font(hour_roller, &roboto_14, 0);
  lv_roller_set_options(hour_roller,
                        "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13"
                        "\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23",
                        LV_ROLLER_MODE_NORMAL);
  if (timeInfo) lv_roller_set_selected(hour_roller, timeInfo->tm_hour, LV_ANIM_OFF);
  lv_roller_set_visible_row_count(hour_roller, 3);
  lv_obj_set_width(hour_roller, 60);

  lv_obj_t *colon = lv_label_create(time_cont);
  lv_label_set_text(colon, ":");
  lv_obj_set_style_text_font(colon, &roboto_bold_14, 0);

  min_roller = lv_roller_create(time_cont);
  lv_obj_set_style_text_font(min_roller, &roboto_14, 0);
  std::string min_opts = "";
  for (int i = 0; i < 60; i++) {
    if (i < 10) min_opts += "0" + std::to_string(i);
    else min_opts += std::to_string(i);
    if (i != 59) min_opts += "\n";
  }

  lv_roller_set_options(min_roller, min_opts.c_str(), LV_ROLLER_MODE_NORMAL);

  if (timeInfo)
    lv_roller_set_selected(min_roller, timeInfo->tm_min, LV_ANIM_OFF);

  lv_roller_set_visible_row_count(min_roller, 3);
  lv_obj_set_width(min_roller, 60);

  lv_obj_t *btn_cont = lv_obj_create(time_step_cont);
  lv_obj_clear_flag(btn_cont, LV_OBJ_FLAG_SCROLLABLE);
  lv_obj_set_layout(btn_cont, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(btn_cont, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(btn_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);
  lv_obj_set_size(btn_cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  lv_obj_set_style_border_width(btn_cont, 0, LV_PART_MAIN);
  lv_obj_set_style_bg_opa(btn_cont, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_pad_column(btn_cont, 15, LV_PART_MAIN);

  lv_obj_t *back_btn = lv_btn_create(btn_cont);
  lv_obj_add_event_cb(back_btn, on_back_clicked, LV_EVENT_CLICKED, this);
  lv_obj_t *back_label = lv_label_create(back_btn);
  lv_label_set_text(back_label, "Wstecz");
  lv_obj_set_style_text_font(back_label, &roboto_14, 0);

  lv_obj_t *save_btn = lv_btn_create(btn_cont);
  lv_obj_add_event_cb(save_btn, on_save_clicked, LV_EVENT_CLICKED, this);
  lv_obj_t *save_label = lv_label_create(save_btn);
  lv_label_set_text(save_label, "Zapisz");
  lv_obj_set_style_text_font(save_label, &roboto_14, 0);
}

void SetTimeScreen::on_back_clicked(lv_event_t *e) {
  ScreenManager::getInstance().pop();
}

void SetTimeScreen::on_save_clicked(lv_event_t *e) {
  SetTimeScreen *view = (SetTimeScreen *)lv_event_get_user_data(e);

  uint32_t hour = lv_roller_get_selected(view->hour_roller);
  uint32_t minute = lv_roller_get_selected(view->min_roller);

  tm t = {};
  t.tm_year = view->selected_year - 1900;
  t.tm_mon = view->selected_month;
  t.tm_mday = view->selected_day;
  t.tm_hour = hour;
  t.tm_min = minute;
  t.tm_sec = 0;
  t.tm_isdst = -1;

  time_t new_time = mktime(&t);

  TimeManager::setTime(new_time);

  auto *stopView = new SelectStop(view->loader);
  
  ScreenManager::getInstance().popSilent();
  ScreenManager::getInstance().popSilent();
  ScreenManager::getInstance().push(stopView);
}

lv_obj_t *SetTimeScreen::getScreen() { return screen; }

void SetTimeScreen::delete_event_handler(lv_event_t *e) {
  auto view = static_cast<SetTimeScreen *>(lv_event_get_user_data(e));
  view->screen = nullptr;
  delete view;
}