/**
 * @file ViewportWindow.h
 * @author Echo 
 * @Date 24-8-17
 * @brief 
 */

#pragma once
#include "WindowBase.h"

#include "ViewportWindow.generated.h"

namespace tool::window {

ECLASS()
class ViewportWindow : public WindowBase {
    GENERATED_CLASS(ViewportWindow)
public:
    ViewportWindow();
    // 是不是主要渲染的对象(交换链)
    bool main = false;

    static void OnWindowResized(int w, int h);
};

}
