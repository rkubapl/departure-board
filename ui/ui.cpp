#include "ui.h"
#include "views/SelectStop.h"

void ui_init(TimetableLoader& loader) {
    SelectStop* stopView = new SelectStop(loader);

    stopView->create();

    lv_screen_load(stopView->getScreen());
}
