#include "ScreenManager.h"
#include <iostream>

#include "ui.h"
#include "views/ErrorView.h"

void ScreenManager::push(IView *newScreen) {
  if (!newScreen)
    return;

  try {
    newScreen->create();

    if (screenStack.empty()) {
      lv_screen_load(newScreen->getScreen());
    } else {
      lv_screen_load_anim(newScreen->getScreen(), LV_SCREEN_LOAD_ANIM_OUT_LEFT,
                          0, 0, false);
    }

    screenStack.push(newScreen);
  } catch (const std::exception &e) {
    std::cerr << "Błąd podczas tworzenia ekranu: " << e.what() << std::endl;

    if (newScreen->getScreen() != nullptr) {
      lv_obj_del_async(newScreen->getScreen());
    } else {
      delete newScreen;
    }

    ErrorView *errorView = new ErrorView("Błąd", e.what());
    this->push(errorView);
  }
}

void ScreenManager::pop() {
  if (screenStack.size() <= 1)
    return;

  IView *currentScreen = screenStack.top();
  screenStack.pop();

  IView *previousScreen = screenStack.top();

  lv_screen_load_anim(previousScreen->getScreen(),
                      LV_SCREEN_LOAD_ANIM_OUT_RIGHT, 0, 0, false);
  lv_obj_del_async(currentScreen->getScreen());
}

void ScreenManager::popSilent() {
  if (screenStack.empty())
    return;

  IView *currentScreen = screenStack.top();
  screenStack.pop();

  lv_obj_del_async(currentScreen->getScreen());
}
