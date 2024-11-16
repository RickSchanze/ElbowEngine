/**
 * @file Input.h
 * @author Echo 
 * @Date 24-5-16
 * @brief 
 */

#pragma once
#include "Core/Math/MathTypes.h"

// clang-format off
enum class KeyCode {
    A = 546,
    B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z,
    Space = 524,
    MouseLeft = 655, MouseRight, MouseMiddle,
    Escape = 526,
};
// clang-format on

class Input {
public:
    static bool IsKeyDown(KeyCode InCode);
    static bool IsKeyPressed(KeyCode InCode);
    static bool IsKeyReleased(KeyCode InCode);
    static Vector2 GetMousePosition();
    static Vector2 GetMouseDelta();
};
