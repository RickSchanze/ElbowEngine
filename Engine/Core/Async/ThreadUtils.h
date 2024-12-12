//
// Created by Echo on 24-12-12.
//

#pragma once
#include "Core/Base/EString.h"

#include <thread>

namespace core {

class ThreadUtils {
public:
    static void SetThreadName(std::thread& thread, core::StringView name);
};

}
