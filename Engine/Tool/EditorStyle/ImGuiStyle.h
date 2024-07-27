/**
 * @file ImGuiStyle.h
 * @author Echo 
 * @Date 24-7-28
 * @brief 
 */

#pragma once

class EditorImGuiStyle
{
public:
    virtual ~EditorImGuiStyle() = default;

    virtual void SetStyle() = 0;
};

class DeepDarkStyle : public EditorImGuiStyle
{
public:
    void SetStyle() override;
};
