#pragma once
#include "IView.h"
#include "TimetableLoader.h"
#include "lvgl/lv_types.h"

class VehicleRoute : public IView {
  Departure departure;
  std::string currentStop;
  std::vector<ArriveStop> stops;
  lv_obj_t* screen;
public:
  VehicleRoute(Departure departure, std::string currentStop, std::vector<ArriveStop> stops);

  void create() override;
  lv_obj_t* getScreen() override;
  void createStop(std::string stopName, uint64_t time, lv_obj_t *list);
private:
  static void back_event_handler(lv_event_t *e);
  static void delete_event_handler(lv_event_t *e);
};
