//
// Created by Jakub Rajchel on 5/30/26.
//

#include "ErrorView.h"

#include "ui.h"

ErrorView::ErrorView(std::string h, std::string m): errorHeader(h), errorMessage(m), screen(nullptr) {}

ErrorView::~ErrorView() {
    if (screen) {
        lv_obj_del(screen);
    }
}

void ErrorView::create() {
    screen = lv_obj_create(NULL);

    lv_obj_set_layout(screen, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(screen, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);

    lv_obj_set_style_pad_all(screen, 0, 0);
    lv_obj_set_style_margin_all(screen, 0, 0);

    lv_obj_t *titleText = lv_label_create(screen);
    lv_label_set_text(titleText, errorHeader.c_str());
    lv_obj_set_style_text_font(titleText, &roboto_bold_14, 0);
    lv_obj_set_style_text_align(titleText, LV_TEXT_ALIGN_CENTER, 0);

    lv_obj_t *message = lv_label_create(screen);
    lv_label_set_text(message, errorMessage.c_str());
    lv_obj_set_style_text_font(message, &roboto_14, 0);
    lv_obj_set_style_text_align(message, LV_TEXT_ALIGN_CENTER, 0);
}

lv_obj_t * ErrorView::getScreen() {
    return screen;
}
