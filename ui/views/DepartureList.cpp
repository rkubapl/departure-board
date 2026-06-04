#include "DepartureList.h"

#include "../ui.h"
#include "VehicleRoute.h"
#include <ctime>

DepartureList::DepartureList(TimetableLoader &loader, std::string stop,
                             int stopId, lv_obj_t *menu)
    : timetableLoader(loader), stop(stop), stopId(stopId), menu(menu),
      screen(nullptr), list(nullptr), refreshTimer(nullptr) {}

DepartureList::~DepartureList() {
  if (screen) {
    lv_obj_del(screen);
  }
}

void DepartureList::delete_event_handler(lv_event_t *e) {
  DepartureList *view = (DepartureList *)lv_event_get_user_data(e);
  if (view->refreshTimer) {
    lv_timer_delete(view->refreshTimer);
    view->refreshTimer = nullptr;
  }
  view->screen = nullptr;
  delete view;
}

void DepartureList::back_event_handler(lv_event_t *e) {
  DepartureList *view = (DepartureList *)lv_event_get_user_data(e);

  lv_screen_load_anim(view->menu, LV_SCREEN_LOAD_ANIM_OUT_RIGHT, 0, 0, false);
  lv_obj_del_async(view->getScreen());
}

void DepartureList::create() { //TODO: add a text that there is no departures for next 24h
  screen = lv_obj_create(NULL);
  lv_obj_set_layout(screen, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_scrollbar_mode(screen, LV_SCROLLBAR_MODE_OFF);

  lv_obj_set_style_pad_all(screen, 0, 0);
  lv_obj_set_style_pad_row(screen, 10, 0);
  lv_obj_set_style_border_width(screen, 0, 0);

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
  lv_obj_add_event_cb(back_btn, back_event_handler, LV_EVENT_CLICKED, this);
  lv_obj_set_style_bg_opa(back_btn, LV_OPA_TRANSP, 0);
  lv_obj_set_style_shadow_width(back_btn, 0, 0);
  lv_obj_set_style_pad_all(back_btn, 2, 0);

  lv_obj_t *back_icon = lv_label_create(back_btn);
  lv_obj_set_style_text_font(back_icon, &lv_font_montserrat_14, 0);
  lv_label_set_text(back_icon, LV_SYMBOL_LEFT);
  lv_obj_set_style_text_color(
      back_icon, lv_obj_get_style_text_color(screen, LV_PART_MAIN), 0);

  lv_obj_t *titleText = lv_label_create(header);
  lv_label_set_text(titleText, stop.c_str());
  lv_obj_set_style_text_font(titleText, &roboto_bold_14, 0);
  lv_label_set_long_mode(titleText, LV_LABEL_LONG_MODE_SCROLL);
  lv_obj_set_flex_grow(titleText, 1);

  list = lv_obj_create(screen);
  lv_obj_set_width(list, lv_pct(100));
  lv_obj_set_flex_grow(list, 1);

  lv_obj_set_style_bg_opa(list, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(list, 0, 0);
  lv_obj_set_style_margin_all(list, 0, 0);
  lv_obj_set_style_pad_all(list, 0, 0);
  lv_obj_set_style_pad_bottom(list, 10, 0);
  lv_obj_set_scrollbar_mode(list, LV_SCROLLBAR_MODE_OFF);

  lv_obj_set_layout(list, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_style_pad_row(list, 5, 0);
  lv_obj_set_flex_align(list, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);

  uint64_t currentUnixTime = std::time(nullptr);

  std::queue<Departure> emptyQueue;
  std::swap(currentDepartures, emptyQueue);

  currentDepartures = timetableLoader.getNextDepartures(stopId, currentUnixTime);
  while (!currentDepartures.empty()) currentDepartures.pop();

  refreshTimer = lv_timer_create(on_timer_refresh, 60000, this);

  if (currentDepartures.empty()) {
    lv_obj_t* text = lv_label_create(list);
    lv_label_set_text(text, "Brak odjazdów przez następne 24h");
    lv_obj_set_style_text_font(text, &roboto_14, 0);
    return;
  }

  std::queue<Departure> temp = currentDepartures;

  for (int i = 0; !temp.empty() && i < 10; i++) {
    createDeparture(temp.front(), list, currentUnixTime);
    temp.pop();
  }
}

lv_obj_t *DepartureList::getScreen() { return screen; }

lv_obj_t *DepartureList::createDeparture(Departure &dep, lv_obj_t *list,
                                         uint64_t currentUnixTime) {
  lv_obj_t *row = lv_obj_create(list);
  lv_obj_add_event_cb(row, on_btn_clicked, LV_EVENT_CLICKED, this);

  lv_obj_set_scrollbar_mode(row, LV_SCROLLBAR_MODE_OFF);
  lv_obj_set_style_margin_all(row, 0, 0);
  lv_obj_set_style_pad_all(row, 10, 0);
  lv_obj_set_width(row, lv_pct(92));
  lv_obj_set_height(row, LV_SIZE_CONTENT);

  lv_obj_set_layout(row, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);

  lv_obj_set_flex_align(row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);

  lv_obj_t *label_line = lv_label_create(row);
  lv_label_set_text(label_line, dep.lineName.c_str());
  lv_obj_set_style_text_color(label_line, lv_color_hex(0x639fff), 0);

  lv_obj_t *label_dest = lv_label_create(row);
  lv_label_set_text(label_dest, dep.destinationName.c_str());
  lv_obj_set_style_text_font(label_dest, &roboto_14, 0);
  lv_obj_set_width(label_dest, 160);
  lv_label_set_long_mode(label_dest, LV_LABEL_LONG_MODE_SCROLL);

  lv_obj_set_style_pad_left(label_dest, 15, 0);

  lv_obj_set_flex_grow(label_dest, 1);

  lv_obj_t *label_time = lv_label_create(row);

  uint64_t minutes = dep.getMinutesToDeparture(currentUnixTime);

  time_t arrTime = dep.departureUnixTime;

  tm *tm_info = localtime(&arrTime);
  char buffer[16];
  strftime(buffer, sizeof(buffer), "%H:%M", tm_info);

  std::string timeStr =
      (minutes == 0
           ? "<1min"
           : (minutes < 15 ? std::to_string(minutes) + "min" : buffer));
  lv_label_set_text(label_time, timeStr.c_str());
  lv_obj_set_style_text_font(label_time, &roboto_light_14, 0);

  return row;
}

void DepartureList::on_btn_clicked(lv_event_t *e) {
  DepartureList *view = static_cast<DepartureList *>(lv_event_get_user_data(e));

  lv_obj_t *row = (lv_obj_t *)lv_event_get_current_target(e);

  int index = lv_obj_get_index(row);

  if (index < 0 || index >= view->currentDepartures.size())
    return;

  std::queue<Departure> temp = view->currentDepartures;
  for (int i = 0; i < index; ++i) {
    temp.pop();
  }
  Departure dep = temp.front();

  std::vector<ArriveStop> stops = view->timetableLoader.getDetailedInfo(
      dep.upcomingStopCount, dep.departureUnixTime, dep.routePatternOffset,
      dep.arrivalTimePatternOffset);

  VehicleRoute *vehicle_route =
      new VehicleRoute(dep, view->stop, stops, view->getScreen());
  vehicle_route->create();
  lv_screen_load_anim(vehicle_route->getScreen(), LV_SCREEN_LOAD_ANIM_OUT_LEFT,
                      0, 0, false);
}

void DepartureList::on_timer_refresh(lv_timer_t *timer) {
  DepartureList *view = (DepartureList *)lv_timer_get_user_data(timer);
  uint64_t currentUnixTime = std::time(nullptr);

  bool changed = false;
  while (!view->currentDepartures.empty() &&
         view->currentDepartures.front().departureUnixTime < currentUnixTime) {
    view->currentDepartures.pop();
    changed = true;
  }

  if (view->currentDepartures.size() < 10) {
    view->currentDepartures =
        view->timetableLoader.getNextDepartures(view->stopId, currentUnixTime);
    changed = true;
  }

  if (changed) {
    lv_obj_clean(view->list);
    std::queue<Departure> temp = view->currentDepartures;
    while (!temp.empty()) {
      view->createDeparture(temp.front(), view->list, currentUnixTime);
      temp.pop();
    }
  } else {
    uint32_t child_cnt = lv_obj_get_child_cnt(view->list);
    std::queue<Departure> temp = view->currentDepartures;
    for (uint32_t i = 0; i < child_cnt; i++) {
      if (temp.empty())
        break;
      lv_obj_t *row = lv_obj_get_child(view->list, i);
      lv_obj_t *label_time = lv_obj_get_child(row, 2);

      uint64_t minutes = temp.front().getMinutesToDeparture(currentUnixTime);
      std::string timeStr =
          (minutes == 0 ? "<1" : std::to_string(minutes)) + "min";
      lv_label_set_text(label_time, timeStr.c_str());

      temp.pop();
    }
  }
}
