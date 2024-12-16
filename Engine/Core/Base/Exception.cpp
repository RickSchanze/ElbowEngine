//
// Created by Echo on 24-12-16.
//

#include "Exception.h"

core::ArgumentException::ArgumentException(core::StringView arg_name, core::StringView msg) :
    Exception(fmt::format("Argument {} Error: {}", arg_name, msg))
{
}