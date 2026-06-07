#include "DepartureList.h"

#include "../ScreenManager.h"
#include "../TimeManager.h"
#include "../ui.h"
#include "VehicleRoute.h"
#include <ctime>

#include "ErrorView.h"

DepartureList::DepartureList(TimetableLoader &loader, std::string stop,
                             int stopId)
    : timetableLoader(loader), stop(stop), stopId(stopId), screen(nullptr),
      list(nullptr), refreshTimer(nullptr) {}

void DepartureList::delete_event_handler(lv_event_t *e) {
  auto *view = static_cast<DepartureList *>(lv_event_get_user_data(e));
  if (view->refreshTimer) {
    lv_timer_delete(view->refreshTimer);
    view->refreshTimer = nullptr;
  }
  view->screen = nullptr;
  delete view;
}

void DepartureList::back_event_handler(lv_event_t *e) {
  ScreenManager::getInstance().pop();
}

void DepartureList::create() {
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
  lv_label_set_text(titleText, stop.c_str());
  lv_obj_set_style_text_font(titleText, &roboto_bold_14, 0);
  lv_label_set_long_mode(titleText, LV_LABEL_LONG_MODE_SCROLL);

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

  time_t t = TimeManager::getTime();
  tm *ltm = localtime(&t);
  uint64_t currentUnixTime = timegm(ltm);
  try {
    currentDepartures =
        timetableLoader.getNextDepartures(stopId, currentUnixTime);
    isOutOfDate = false;
  } catch (const ScheduleOutOfDateException &e) {
    currentDepartures.clear();
    isOutOfDate = true;
  }

  refreshTimer = lv_timer_create(on_timer_refresh, 60000, this);

  renderDepartures();
}

void DepartureList::renderDepartures() {
  time_t t = TimeManager::getTime();
  tm *ltm = localtime(&t);
  uint64_t currentUnixTime = timegm(ltm);

  lv_obj_clean(list);

  if (isOutOfDate) {
    lv_obj_t *text = lv_label_create(list);
    lv_label_set_text(text, "Dane rozkładu są nieaktualne.\nZaktualizuj plik z rozkładem.");
    lv_obj_set_style_text_font(text, &roboto_14, 0);
    lv_obj_set_style_text_align(text, LV_TEXT_ALIGN_CENTER, 0);
    return;
  }

  if (currentDepartures.empty()) {
    lv_obj_t *text = lv_label_create(list);
    lv_label_set_text(text, "Brak odjazdów przez następne 24h");
    lv_obj_set_style_text_font(text, &roboto_14, 0);
    lv_obj_set_style_text_align(text, LV_TEXT_ALIGN_CENTER, 0);
    return;
  }

  int i = 0;
  for (auto it = currentDepartures.begin(); it != currentDepartures.end() && i < 10; ++it, ++i) {
    createDeparture(*it, list, currentUnixTime);
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
  lv_label_set_text(label_line, dep.getLineName().c_str());
  lv_obj_set_style_text_color(label_line, lv_color_hex(0x639fff), 0);

  lv_obj_t *label_dest = lv_label_create(row);
  lv_label_set_text(label_dest, dep.getDestinationName().c_str());
  lv_obj_set_style_text_font(label_dest, &roboto_14, 0);
  lv_obj_set_width(label_dest, 160);
  lv_label_set_long_mode(label_dest, LV_LABEL_LONG_MODE_SCROLL);
  lv_obj_set_style_pad_left(label_dest, 15, 0);
  lv_obj_set_flex_grow(label_dest, 1);

  lv_obj_t *label_time = lv_label_create(row);
  lv_label_set_text(label_time, formatTime(dep, currentUnixTime).c_str());
  lv_obj_set_style_text_font(label_time, &roboto_light_14, 0);

  return row;
}

std::string DepartureList::formatTime(Departure &dep, uint64_t currentUnixTime) {
  uint64_t minutes = dep.getMinutesToDeparture(currentUnixTime);

  if (minutes == 0) {
    return "<1min";
  }
  if (minutes < 15) {
    return std::to_string(minutes) + "min";
  }

  auto arrTime = static_cast<time_t>(dep.getDepartureUnixTime());
  tm *tm_info = gmtime(&arrTime);
  char buffer[16];
  strftime(buffer, sizeof(buffer), "%H:%M", tm_info);
  return buffer;
}

void DepartureList::on_btn_clicked(lv_event_t *e) {
  auto *view = static_cast<DepartureList *>(lv_event_get_user_data(e));
  auto *row = static_cast<lv_obj_t *>(lv_event_get_current_target(e));

  int index = lv_obj_get_index(row);

  if (index < 0 || index >= static_cast<int>(view->currentDepartures.size()))
    return;

  auto it = std::next(view->currentDepartures.begin(), index);
  const Departure &dep = *it;

  std::vector<ArriveStop> stops = view->timetableLoader.getDetailedInfo(dep);

  auto *vehicle_route = new VehicleRoute(dep, view->stop, stops);
  ScreenManager::getInstance().push(vehicle_route);
}

void DepartureList::on_timer_refresh(lv_timer_t *timer) {
  auto *view = static_cast<DepartureList *>(lv_timer_get_user_data(timer));
  time_t t = TimeManager::getTime();
  tm *ltm = localtime(&t);
  uint64_t currentUnixTime = timegm(ltm);

  bool changed = false;
  while (!view->currentDepartures.empty() && view->currentDepartures.front().getDepartureUnixTime() < currentUnixTime) {
    view->currentDepartures.pop_front();
    changed = true;
  }

  if (view->currentDepartures.size() < 10) {
    try {
      view->currentDepartures = view->timetableLoader.getNextDepartures(
          view->stopId, currentUnixTime);
      view->isOutOfDate = false;
    } catch (const ScheduleOutOfDateException &e) {
      view->currentDepartures.clear();
      view->isOutOfDate = true;
    } catch (const std::exception &e) {
      ScreenManager::getInstance().pop(); // deletes screen asynchronously
      auto *error_view = new ErrorView("Błąd", e.what());
      ScreenManager::getInstance().push(error_view);
      return;
    }

    changed = true;
  }

  if (changed) {
    view->renderDepartures();
  } else if (!view->currentDepartures.empty()) {
    uint32_t child_cnt = lv_obj_get_child_cnt(view->list);
    auto it = view->currentDepartures.begin();
    for (uint32_t i = 0; i < child_cnt && it != view->currentDepartures.end();
         i++, ++it) {
      lv_obj_t *row = lv_obj_get_child(view->list, i);

      lv_obj_t *label_time = lv_obj_get_child(row, 2);
      lv_label_set_text(label_time, formatTime(*it, currentUnixTime).c_str());
    }
  }
}
