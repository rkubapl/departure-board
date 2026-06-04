//
// Created by Jakub Rajchel on 5/30/26.
//

#ifndef DEPARTUREDISPLAY_ERRORVIEW_H
#define DEPARTUREDISPLAY_ERRORVIEW_H
#include <string>

#include "IView.h"


class ErrorView: public IView {
    std::string errorHeader;
    std::string errorMessage;
    lv_obj_t* screen;

public:
    ErrorView(std::string h, std::string m);
    ~ErrorView() override;

    void create() override;
    lv_obj_t* getScreen() override;
};


#endif //DEPARTUREDISPLAY_ERRORVIEW_H