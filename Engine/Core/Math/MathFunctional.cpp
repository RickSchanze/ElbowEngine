//
// Created by Echo on 25-2-6.
//

#include "MathFunctional.h"

using namespace core;
using namespace impl;

Vector2 impl::operator|(Vector3 v, ToVector2_Impl to_vector2_impl) { return {v.x, v.y}; }