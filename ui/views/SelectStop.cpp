
#include "SelectStop.h"

#include "DepartureList.h"
#include "ui.h"
#include "../ScreenManager.h"
#include "lvgl/api_map/lv_api_map_v8.h"
#include "lvgl/widgets/lv_label.h"
#include "lvgl/widgets/lv_list.h"

#define QUESTION_MARK_ICON "\xEF\x81\x99"

SelectStop::SelectStop(TimetableLoader &loader)
    : timetableLoader(loader), screen(nullptr) {}

void SelectStop::delete_event_handler(lv_event_t *e) {
  auto *view = static_cast<SelectStop*>(lv_event_get_user_data(e));
  view->screen = nullptr;
  delete view;
}

void SelectStop::create() { //TODO: if the schedule doesn't cover current date then add information that there is no data
  screen = lv_obj_create(NULL);
  lv_obj_add_event_cb(screen, delete_event_handler, LV_EVENT_DELETE, this);

  lv_obj_set_layout(screen, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_style_pad_all(screen, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_top(screen, 5, LV_PART_MAIN);
  lv_obj_t *titleText = lv_label_create(screen);
  lv_label_set_text(titleText, "Wybierz przystanek");
  lv_obj_set_style_text_font(titleText, &roboto_bold_14, 0);
  lv_obj_set_width(titleText, lv_pct(100));
  lv_obj_set_style_text_align(titleText, LV_TEXT_ALIGN_CENTER, 0);

  lv_obj_t *list = lv_list_create(screen);
  lv_obj_set_width(list, lv_pct(100));
  lv_obj_set_flex_grow(list, 1);

  lv_obj_set_style_border_width(list, 0, LV_PART_MAIN);
  lv_obj_set_style_radius(list, 0, LV_PART_MAIN);

  for (auto &stop : timetableLoader.getStopsToSelect()) {
    lv_obj_t *btn = lv_list_add_btn(list, LV_SYMBOL_GPS, stop.first.c_str());
    lv_obj_set_style_text_font(btn, &roboto_14, 0);
    lv_obj_add_event_cb(btn, on_btn_clicked, LV_EVENT_CLICKED, this);
  }
}

void SelectStop::on_btn_clicked(lv_event_t *e) {
  lv_obj_t *btn = (lv_obj_t *)lv_event_get_target(e);
  SelectStop *view = (SelectStop *)lv_event_get_user_data(e);
  lv_obj_t *list = lv_obj_get_parent(btn);

  const char *stop_name = lv_list_get_btn_text(list, btn);

  int stop_id = 0;
  const auto &stops = view->timetableLoader.getStopsToSelect();
  for (size_t i = 0; i < stops.size(); ++i) {
    if (stops[i].first == stop_name) {
      stop_id = i;
      break;
    }
  }

  DepartureList *departure_list = new DepartureList(view->timetableLoader, std::string(stop_name), stop_id);
  ScreenManager::getInstance().push(departure_list);
}

lv_obj_t *SelectStop::getScreen() { return screen; }