/**
 * @file Input.h
 * @author Echo 
 * @Date 24-5-16
 * @brief 
 */

#pragma once
#include "Math/MathTypes.h"

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
    static void InternalTick();

public:
    static bool IsKeyDown(KeyCode InCode);
    static bool IsKeyPressed(KeyCode InCode);
    static bool IsKeyReleased(KeyCode InCode);
    static FVector2 GetMousePosition();
    static FVector2 GetMouseDelta();

private:
    static inline FVector2 sLastFrameMousePos;
};
