//
// Created by Jakub Rajchel on 5/30/26.
//

#include "ErrorView.h"

#include "ui.h"

#include "../ScreenManager.h"

ErrorView::ErrorView(std::string h, std::string m)
    : errorHeader(h), errorMessage(m), screen(nullptr) {}


void ErrorView::create() {
  screen = lv_obj_create(NULL);
  lv_obj_add_event_cb(screen, delete_event_handler, LV_EVENT_DELETE, this);
  lv_obj_set_style_bg_color(screen, lv_color_hex(0x121212), 0);
  lv_obj_set_style_pad_all(screen, 20, 0);

  lv_obj_set_layout(screen, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(screen, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
                        LV_FLEX_ALIGN_CENTER);

  lv_obj_t *icon = lv_label_create(screen);
  lv_label_set_text(icon, LV_SYMBOL_WARNING);
  lv_obj_set_style_text_color(icon, lv_color_hex(0xff5555), 0);
  lv_obj_set_style_text_font(icon, &roboto_bold_14, 0);

  lv_obj_t *titleText = lv_label_create(screen);
  lv_label_set_text(titleText, errorHeader.c_str());
  lv_obj_set_style_text_font(titleText, &roboto_bold_14, 0);
  lv_obj_set_style_text_color(titleText, lv_color_hex(0xffffff), 0);
  lv_obj_set_style_margin_top(titleText, 10, 0);

  lv_obj_t *message = lv_label_create(screen);
  lv_label_set_text(message, errorMessage.c_str());
  lv_obj_set_style_text_font(message, &roboto_14, 0);
  lv_obj_set_style_text_color(message, lv_color_hex(0xaaaaaa), 0);
  lv_obj_set_style_text_align(message, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_set_width(message, lv_pct(100));
  lv_label_set_long_mode(message, LV_LABEL_LONG_MODE_WRAP);
  lv_obj_set_style_margin_top(message, 10, 0);

  if (ScreenManager::getInstance().hasPreviousScreens()) {
    lv_obj_t *backBtn = lv_button_create(screen);
    lv_obj_add_event_cb(backBtn, back_event_handler, LV_EVENT_CLICKED, this);
    lv_obj_set_style_margin_top(backBtn, 30, 0);

    lv_obj_t *backLabel = lv_label_create(backBtn);
    lv_label_set_text(backLabel, "Zamknij");
    lv_obj_set_style_text_font(backLabel, &roboto_bold_14, 0);
  }
}

void ErrorView::back_event_handler(lv_event_t *e) {
  ScreenManager::getInstance().pop();
}

lv_obj_t *ErrorView::getScreen() { return screen; }


void ErrorView::delete_event_handler(lv_event_t *e) {
  auto *view = static_cast<ErrorView *>(lv_event_get_user_data(e));
  view->screen = nullptr;
  delete view;
}