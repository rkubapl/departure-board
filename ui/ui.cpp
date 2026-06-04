#include "ui.h"

#include "views/ErrorView.h"
#include "views/SelectStop.h"
#include "views/SetTimeScreen.h"

void ui_init(time_t initialTime, TimetableLoader &loader) {
  try {
    loader.load();
  } catch (const std::exception &e) {
    ui_error_screen("Błąd ładowania pliku", e.what());
    return;
  }

  auto setTimeScreen = new SetTimeScreen(initialTime, loader);
  setTimeScreen->create();

  lv_screen_load(setTimeScreen->getScreen());
}

void ui_error_screen(std::string header, std::string message) {
  auto errorView = new ErrorView(header, message);
  errorView->create();

  lv_screen_load(errorView->getScreen());
}