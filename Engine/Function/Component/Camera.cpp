/**
 * @file CameraComponent.cpp
 * @author Echo 
 * @Date 24-5-16
 * @brief 
 */

#include "Camera.h"

#include "CoreGlobal.h"
#include "Input/Input.h"
#include "TransformComponent.h"

void Function::Camera::Tick() {
    Component::Tick();
    if (bEnableInput) {
        if (Input::IsKeyDown(KeyCode::W)) {
            mTransform->Position += mTransform->Rotation.GetForwardVector() * MovementSpeed;
        }
    }
}

void Function::Camera::BeginPlay() {
    Super::BeginPlay();
    // TODO
    mTransform = new TransformComponent();
    if (Main == nullptr) {
        Main = this;
    }
}
