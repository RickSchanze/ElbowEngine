//
// Created by Echo on 2025/3/25.
//

#include "Transform.hpp"

IMPL_REFLECTED(Transform) {
    return Type::Create<Transform>("Transform") | refl_helper::AddField("location", &Transform::location) |
           refl_helper::AddField("scale", &Transform::scale) | refl_helper::AddField("rotation", &Transform::rotation);
}
