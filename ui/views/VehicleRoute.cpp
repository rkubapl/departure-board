#include "VehicleRoute.h"
#include <ctime>

#include "../ScreenManager.h"
#include "../ui.h"
#include "lvgl/api_map/lv_api_map_v8.h"
#include "lvgl/core/lv_obj.h"
#include "lvgl/widgets/lv_button.h"
#include "lvgl/widgets/lv_label.h"

VehicleRoute::VehicleRoute(Departure departure, std::string currentStop,
                           std::vector<ArriveStop> stops)
    : departure(departure), currentStop(currentStop), stops(stops),
      screen(nullptr) {}

void VehicleRoute::delete_event_handler(lv_event_t *e) {
  auto view = static_cast<VehicleRoute *>(lv_event_get_user_data(e));
  view->screen = nullptr;
  delete view;
}

void VehicleRoute::back_event_handler(lv_event_t *e) {
  ScreenManager::getInstance().pop();
}

void VehicleRoute::create() {
  screen = lv_obj_create(NULL);
  lv_obj_set_layout(screen, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_scrollbar_mode(screen, LV_SCROLLBAR_MODE_OFF);

  lv_obj_set_style_pad_all(screen, 0, 0);
  lv_obj_set_style_pad_row(screen, 0, 0);
  lv_obj_set_style_border_width(screen, 0, 0);
  lv_obj_set_style_pad_row(screen, 10, 0);
  lv_obj_add_event_cb(screen, delete_event_handler, LV_EVENT_DELETE, this);

  lv_obj_t *header = lv_obj_create(screen);
  lv_obj_set_width(header, lv_pct(100));
  lv_obj_set_height(header, LV_SIZE_CONTENT);
  lv_obj_set_style_bg_opa(header, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(header, 0, 0);
  lv_obj_set_style_margin_all(header, 0, 0);
  lv_obj_set_style_pad_hor(header, 10, 0);
  lv_obj_set_style_pad_top(header, 10, 0);
  lv_obj_set_style_pad_bottom(header, 0, 0);
  lv_obj_set_scrollbar_mode(header, LV_SCROLLBAR_MODE_OFF);

  lv_obj_set_layout(header, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(header, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);

  lv_obj_t *back_btn = lv_button_create(header);
  lv_obj_set_width(back_btn, LV_SIZE_CONTENT);
  lv_obj_set_height(back_btn, LV_SIZE_CONTENT);
  lv_obj_set_style_max_width(back_btn, lv_pct(100), 0);
  lv_obj_add_event_cb(back_btn, back_event_handler, LV_EVENT_CLICKED, this);
  lv_obj_set_style_bg_opa(back_btn, LV_OPA_TRANSP, 0);
  lv_obj_set_style_shadow_width(back_btn, 0, 0);
  lv_obj_set_style_pad_all(back_btn, 0, 0);

  lv_obj_set_layout(back_btn, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(back_btn, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(back_btn, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);

  lv_obj_t *back_icon = lv_label_create(back_btn);
  lv_obj_set_style_text_font(back_icon, &lv_font_montserrat_14, 0);
  lv_label_set_text(back_icon, LV_SYMBOL_LEFT);
  lv_obj_set_style_text_color(
      back_icon, lv_obj_get_style_text_color(screen, LV_PART_MAIN), 0);
  lv_obj_set_style_pad_all(back_icon, 2, 0);

  lv_obj_t *titleText = lv_label_create(back_btn);
  std::string titleStr =
      departure.getLineName() + " " + departure.getDestinationName();
  lv_label_set_text(titleText, titleStr.c_str());
  lv_obj_set_style_text_font(titleText, &roboto_bold_14, 0);
  lv_label_set_long_mode(titleText, LV_LABEL_LONG_MODE_SCROLL);
  // lv_obj_set_style_pad_left(titleText, 5, 0);

  lv_obj_t *list = lv_obj_create(screen);
  lv_obj_set_width(list, lv_pct(100));
  lv_obj_set_flex_grow(list, 1);

  lv_obj_set_style_bg_opa(list, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(list, 0, 0);
  lv_obj_set_style_margin_all(list, 0, 0);
  lv_obj_set_style_pad_all(list, 0, 0);
  lv_obj_set_style_pad_bottom(list, 10, 0);
  // lv_obj_set_scrollbar_mode(list, LV_SCROLLBAR_MODE_OFF);

  lv_obj_set_layout(list, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_style_pad_row(list, 0, 0);
  lv_obj_set_flex_align(list, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START,
                        LV_FLEX_ALIGN_START);

  lv_obj_t *text = lv_label_create(list);
  lv_obj_set_width(text, lv_pct(100));
  lv_obj_set_style_pad_all(text, 0, 0);
  lv_obj_set_style_margin_top(text, 5, 0);
  lv_obj_set_style_margin_bottom(text, 10, 0);
  lv_label_set_text(text, "Trasa pojazdu");
  lv_obj_set_style_text_align(text, LV_TEXT_ALIGN_CENTER, 0);

  createStop(currentStop, departure.getDepartureUnixTime(), list);

  for (size_t i = 0; i < stops.size(); ++i) {
    lv_obj_t *line = lv_obj_create(list);
    lv_obj_set_size(line, 3, 20);
    lv_obj_set_style_bg_color(
        line, lv_obj_get_style_text_color(screen, LV_PART_MAIN), 0);
    lv_obj_set_style_bg_opa(line, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(line, 0, 0);
    lv_obj_set_style_margin_left(line, 37, 0);
    lv_obj_set_style_margin_top(line, 0, 0);
    lv_obj_set_style_margin_bottom(line, 0, 0);
    lv_obj_set_style_pad_all(line, 0, 0);
    lv_obj_set_style_radius(line, 0, 0);

    createStop(stops[i].first, stops[i].second, list);
  }
}

lv_obj_t *VehicleRoute::getScreen() { return screen; }

void VehicleRoute::createStop(std::string stopName, uint64_t time,
                              lv_obj_t *list) {
  lv_obj_t *row = lv_obj_create(list);
  lv_obj_set_scrollbar_mode(row, LV_SCROLLBAR_MODE_OFF);
  lv_obj_set_style_margin_all(row, 0, 0);
  lv_obj_set_style_pad_hor(row, 10, 0);
  lv_obj_set_style_pad_ver(row, 5, 0);
  lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(row, 0, 0);
  lv_obj_set_width(row, lv_pct(100));
  lv_obj_set_height(row, LV_SIZE_CONTENT);

  lv_obj_set_layout(row, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);

  lv_obj_set_flex_align(row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);

  lv_obj_t *label_time = lv_label_create(row);

  time_t arrTime = time;
  struct tm *tm_info = gmtime(&arrTime);
  char buffer[16];
  strftime(buffer, sizeof(buffer), "%H:%M", tm_info);

  lv_label_set_text(label_time, buffer);
  lv_obj_set_style_text_font(label_time, &roboto_light_14, 0);
  lv_obj_set_style_pad_hor(label_time, 10, 0);
  lv_obj_set_style_text_align(label_time, LV_TEXT_ALIGN_CENTER, 0);

  lv_obj_t *label_dest = lv_label_create(row);
  lv_label_set_text(label_dest, stopName.c_str());
  lv_label_set_long_mode(label_dest, LV_LABEL_LONG_MODE_SCROLL);
  lv_obj_set_style_text_font(label_dest, &roboto_14, 0);
  lv_obj_set_flex_grow(label_dest, 1);
  // lv_obj_set_style_pad_left(label_dest, 10, 0);
}
