/**
 * @file IDetailGUIDrawer.h
 * @author Echo 
 * @Date 24-7-21
 * @brief 
 */

#pragma once
#include "Core/CoreDef.h"

namespace function {

/**
 * 继承此接口的类 ImGui在绘制时会使用此接口返回的方法而不是默认方法
 */
class IDetailGUIDrawer
{
    RTTR_ENABLE()
public:
    virtual ~IDetailGUIDrawer() = default;

    virtual void OnInspectorGUI() INTERFACE_METHOD
};

}
