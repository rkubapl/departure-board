#pragma once
#include <lvgl.h>

class IView {
public:
  virtual ~IView() = default;
  virtual void create() = 0;
  virtual lv_obj_t* getScreen() = 0;
};