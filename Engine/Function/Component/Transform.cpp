/**
 * @file TransformComponent.cpp
 * @author Echo 
 * @Date 24-5-15
 * @brief 
 */

#include "Transform.h"
#include "Transform.generated.h"

GENERATED_SOURCE()

Vector3 Function::Transform::GetForwardVector() const {
    return Rotation.GetForwardVector();
}
Vector3 Function::Transform::GetUpVector() const {
    return Rotation.GetUpVector();
}

Vector3 Function::Transform::GetRightVector() const {
    return Rotation.GetRightVector();
}
