#pragma once
#include "IView.h"
#include "TimetableLoader.h"
#include "lvgl/lv_types.h"

class DepartureList : public IView {
  TimetableLoader &timetableLoader;
  std::string stop;
  int stopId;
  lv_obj_t *screen;
  lv_obj_t *list;
  lv_timer_t *refreshTimer;
  std::list<Departure> currentDepartures;
  bool isOutOfDate = false;

  void renderDepartures();
  lv_obj_t *createDeparture(Departure &dep, lv_obj_t *list,
                            uint64_t currentUnixTime);
  static std::string formatTime(Departure &dep, uint64_t currentUnixTime);

public:
  DepartureList(TimetableLoader &loader, std::string stop, int stopId);

  ~DepartureList() override = default;
  void create() override;
  lv_obj_t *getScreen() override;

private:
  static void on_btn_clicked(lv_event_t *e);
  static void back_event_handler(lv_event_t *e);
  static void delete_event_handler(lv_event_t *e);
  static void on_timer_refresh(lv_timer_t *timer);
};
