#pragma once
#include "IView.h"
#include "TimetableLoader.h"
#include "lvgl/lv_types.h"

class SetTimeScreen : public IView {
    uint64_t initialTime;
    TimetableLoader& loader;
    lv_obj_t* screen;
    lv_obj_t* calendar;
    lv_obj_t* hour_roller;
    lv_obj_t* min_roller;

    static void on_save_clicked(lv_event_t * e);
public:
    SetTimeScreen(uint64_t initialTime, TimetableLoader& loader);

    void create() override;
    lv_obj_t* getScreen() override;
};

