#pragma once
#include "IView.h"
#include "TimetableLoader.h"
#include "lvgl/lv_types.h"

class SelectStop: public IView {
    TimetableLoader& timetableLoader;
    lv_obj_t* screen;

public:
    SelectStop(TimetableLoader& loader);
    ~SelectStop() override;

    void create() override;
    lv_obj_t* getScreen() override;
private:
    static void on_btn_clicked(lv_event_t *e);
};
