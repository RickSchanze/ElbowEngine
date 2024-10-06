/**
 * @file Input.cpp
 * @author Echo 
 * @Date 24-5-16
 * @brief 
 */

#include "Input.h"

#include "imgui.h"

bool Input::IsKeyDown(KeyCode InCode)
{
    return ImGui::IsKeyDown(static_cast<ImGuiKey>(static_cast<int>(InCode)));
}

bool Input::IsKeyPressed(KeyCode InCode)
{
    return ImGui::IsKeyPressed(static_cast<ImGuiKey>(static_cast<int>(InCode)));
}

bool Input::IsKeyReleased(KeyCode InCode)
{
    return ImGui::IsKeyReleased(static_cast<ImGuiKey>(static_cast<int>(InCode)));
}

Vector2 Input::GetMousePosition()
{
    ImGuiIO& IO = ImGui::GetIO();
    return {IO.MousePos.x, IO.MousePos.y};
}

Vector2 Input::GetMouseDelta()
{
    auto& io = ImGui::GetIO();
    return {io.MouseDelta.x, io.MouseDelta.y};
}
