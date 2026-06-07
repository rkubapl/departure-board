#pragma once

#include "views/IView.h"
#include <lvgl.h>
#include <stack>
#include <stdexcept>

class ScreenManager {
  std::stack<IView *> screenStack;
  ScreenManager() = default;

public:
  static ScreenManager &getInstance() {
    static ScreenManager instance;
    return instance;
  }

  ScreenManager(const ScreenManager &) = delete;
  ScreenManager &operator=(const ScreenManager &) = delete;

  void push(IView *newScreen);
  void pop();
  void popSilent();
  bool hasPreviousScreens() const { return !screenStack.empty(); }
};
