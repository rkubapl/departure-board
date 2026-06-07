#include "ui.h"

#include "ScreenManager.h"
#include "views/ErrorView.h"
#include "views/SelectStop.h"
#include "views/SetDateScreen.h"
#include "views/SetTimeScreen.h"

void ui_init(TimetableLoader &loader) {
  auto setDateScreen = new SetDateScreen(loader);
  ScreenManager::getInstance().push(setDateScreen);
}

void ui_error_screen(std::string header, std::string message) {
  auto errorView = new ErrorView(header, message);
  errorView->create();

  lv_screen_load(errorView->getScreen());
}