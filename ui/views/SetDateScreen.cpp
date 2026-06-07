#include "SetDateScreen.h"
#include "SetTimeScreen.h"
#include "../ScreenManager.h"
#include "../TimeManager.h"
#include "ui.h"
#include <ctime>
#include <string>

SetDateScreen::SetDateScreen(TimetableLoader &loader)
    : loader(loader), screen(nullptr), day_roller(nullptr),
      month_roller(nullptr), year_roller(nullptr) {
}

void SetDateScreen::create() {
    loader.load();

    screen = lv_obj_create(NULL);
    lv_obj_add_event_cb(screen, delete_event_handler, LV_EVENT_DELETE, this);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_set_layout(screen, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(screen, 10, LV_PART_MAIN);
    lv_obj_set_flex_align(screen, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *titleText = lv_label_create(screen);
    lv_label_set_text(titleText, "Wybierz datę");
    lv_obj_set_style_text_font(titleText, &roboto_bold_14, 0);
    lv_obj_set_width(titleText, lv_pct(100));
    lv_obj_set_style_text_align(titleText, LV_TEXT_ALIGN_CENTER, 0);

    auto timeObj = TimeManager::getTime();
    tm *timeInfo = localtime(&timeObj);

    lv_obj_t *date_step_cont = lv_obj_create(screen);
    lv_obj_clear_flag(date_step_cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_width(date_step_cont, lv_pct(100));
    lv_obj_set_flex_grow(date_step_cont, 1);
    lv_obj_set_layout(date_step_cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(date_step_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(date_step_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_border_width(date_step_cont, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(date_step_cont, LV_OPA_TRANSP, LV_PART_MAIN);

    lv_obj_t *date_cont = lv_obj_create(date_step_cont);
    lv_obj_clear_flag(date_cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_layout(date_cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(date_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(date_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_size(date_cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_border_width(date_cont, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(date_cont, LV_OPA_TRANSP, LV_PART_MAIN);

    day_roller = lv_roller_create(date_cont);
    lv_obj_set_style_text_font(day_roller, &roboto_14, 0);
    std::string day_opts = "";
    for (int i = 1; i <= 31; i++) {
        if (i < 10) day_opts += "0";
        day_opts += std::to_string(i);
        if (i != 31) day_opts += "\n";
    }

    lv_roller_set_options(day_roller, day_opts.c_str(), LV_ROLLER_MODE_NORMAL);
    if (timeInfo) lv_roller_set_selected(day_roller, timeInfo->tm_mday - 1, LV_ANIM_OFF);
    lv_roller_set_visible_row_count(day_roller, 3);
    lv_obj_set_width(day_roller, 60);

    lv_obj_t *dot1 = lv_label_create(date_cont);
    lv_label_set_text(dot1, ".");
    lv_obj_set_style_text_font(dot1, &roboto_bold_14, 0);

    month_roller = lv_roller_create(date_cont);
    lv_obj_set_style_text_font(month_roller, &roboto_14, 0);
    const char *month_opts =
            "Styczeń\nLuty\nMarzec\nKwiecień\nMaj\nCzerwiec\nLipiec\nSierpień\nWrzesi"
            "eń\nPaździernik\nListopad\nGrudzień";
    lv_roller_set_options(month_roller, month_opts, LV_ROLLER_MODE_NORMAL);
    if (timeInfo) lv_roller_set_selected(month_roller, timeInfo->tm_mon, LV_ANIM_OFF);
    lv_roller_set_visible_row_count(month_roller, 3);
    lv_obj_set_width(month_roller, 100);
    lv_obj_add_event_cb(month_roller, on_month_or_year_changed, LV_EVENT_VALUE_CHANGED, this);

    lv_obj_t *dot2 = lv_label_create(date_cont);
    lv_label_set_text(dot2, ".");
    lv_obj_set_style_text_font(dot2, &roboto_bold_14, 0);

    year_roller = lv_roller_create(date_cont);
    lv_obj_set_style_text_font(year_roller, &roboto_14, 0);
    std::string year_opts = "";
    int current_year = timeInfo ? timeInfo->tm_year + 1900 : 2026;
    for (int i = -10; i < 10; i++) {
        year_opts += std::to_string(current_year + i);
        if (i != 9) year_opts += "\n";
    }
    lv_roller_set_options(year_roller, year_opts.c_str(), LV_ROLLER_MODE_NORMAL);
    lv_roller_set_selected(year_roller, 10, LV_ANIM_OFF);
    lv_roller_set_visible_row_count(year_roller, 3);
    lv_obj_set_width(year_roller, 80);
    lv_obj_add_event_cb(year_roller, on_month_or_year_changed, LV_EVENT_VALUE_CHANGED, this);

    update_days_roller(this);

    lv_obj_t *next_btn = lv_btn_create(date_step_cont);
    lv_obj_add_event_cb(next_btn, on_next_clicked, LV_EVENT_CLICKED, this);
    lv_obj_t *next_label = lv_label_create(next_btn);
    lv_label_set_text(next_label, "Dalej");
    lv_obj_set_style_text_font(next_label, &roboto_14, 0);
}

void SetDateScreen::on_next_clicked(lv_event_t *e) {
    SetDateScreen *view = (SetDateScreen *) lv_event_get_user_data(e);

    uint32_t day_idx = lv_roller_get_selected(view->day_roller);
    uint32_t month_idx = lv_roller_get_selected(view->month_roller);

    char year_str[10];
    lv_roller_get_selected_str(view->year_roller, year_str, sizeof(year_str));
    int year = std::stoi(year_str);

    int day = day_idx + 1;
    int month = month_idx;

    auto *timeView = new SetTimeScreen(view->loader, year, month, day);
    ScreenManager::getInstance().push(timeView);
}

lv_obj_t *SetDateScreen::getScreen() { return screen; }

void SetDateScreen::delete_event_handler(lv_event_t *e) {
    auto view = static_cast<SetDateScreen *>(lv_event_get_user_data(e));
    view->screen = nullptr;
    delete view;
}

int SetDateScreen::get_days_in_month(int year, int month) {
    if (month == 2) {
        if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) return 29;
        return 28;
    }
    if (month == 4 || month == 6 || month == 9 || month == 11) return 30;
    return 31;
}

void SetDateScreen::update_days_roller(SetDateScreen *view) {
    char year_str[10];
    lv_roller_get_selected_str(view->year_roller, year_str, sizeof(year_str));
    int year = std::stoi(year_str);

    int month = lv_roller_get_selected(view->month_roller) + 1;

    int days = get_days_in_month(year, month);

    std::string day_opts = "";
    for (int i = 1; i <= days; i++) {
        if (i < 10) day_opts += "0";
        day_opts += std::to_string(i);
        if (i != days) day_opts += "\n";
    }

    int current_day = lv_roller_get_selected(view->day_roller);
    if (current_day >= days) current_day = days - 1;

    lv_roller_set_options(view->day_roller, day_opts.c_str(), LV_ROLLER_MODE_NORMAL);
    lv_roller_set_selected(view->day_roller, current_day, LV_ANIM_OFF);
}

void SetDateScreen::on_month_or_year_changed(lv_event_t *e) {
    auto view = static_cast<SetDateScreen *>(lv_event_get_user_data(e));
    update_days_roller(view);
}
