//
// Created by Echo on 2025/3/21.
//
#pragma once
#include "nlohmann/json.hpp"

#include "Any.hpp"
#include "Field.hpp"
#include "Helper.hpp"
#include "ReflManager.hpp"
#include "Type.hpp"

nlohmann::json ParseSubAttr(const StringView attr);

void* CreateFromType(const Type* t);
