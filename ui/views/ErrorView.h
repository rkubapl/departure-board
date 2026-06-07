//
// Created by Jakub Rajchel on 5/30/26.
//

#ifndef DEPARTUREDISPLAY_ERRORVIEW_H
#define DEPARTUREDISPLAY_ERRORVIEW_H
#include <string>

#include "IView.h"

class ErrorView : public IView {
  std::string errorHeader;
  std::string errorMessage;
  lv_obj_t *screen;

public:
  ErrorView(std::string h, std::string m);
  ~ErrorView() override = default;

  void create() override;
  lv_obj_t *getScreen() override;

private:
  static void back_event_handler(lv_event_t *e);
  static void delete_event_handler(lv_event_t *e);
};

#endif // DEPARTUREDISPLAY_ERRORVIEW_H