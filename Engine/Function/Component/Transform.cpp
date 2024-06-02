/**
 * @file TransformComponent.cpp
 * @author Echo 
 * @Date 24-5-15
 * @brief 
 */

#include "Transform.h"

GENERATED_SOURCE()

FVector3 Function::Transform::GetForwardVector() const {
    return Rotation.GetForwardVector();
}
FVector3 Function::Transform::GetUpVector() const {
    return Rotation.GetUpVector();
}

FVector3 Function::Transform::GetRightVector() const {
    return Rotation.GetRightVector();
}
