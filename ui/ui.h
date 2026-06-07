#pragma once
#include "TimetableLoader.h"
#include "lvgl/font/lv_font.h"

LV_FONT_DECLARE(roboto_14)
LV_FONT_DECLARE(roboto_bold_14)
LV_FONT_DECLARE(roboto_light_14)
LV_FONT_DECLARE(question_mark)

void ui_init(TimetableLoader& loader);
void ui_error_screen(std::string header, std::string message);

