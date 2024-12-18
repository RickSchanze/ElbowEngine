//
// Created by Echo on 24-12-16.
//

#pragma once

#include "EString.h"
#include <exception>

namespace core
{
class Exception : std::exception
{
public:
    explicit Exception(const core::String& msg);

    [[nodiscard]] char const* what() const override { return message_.Data(); }

protected:
    String message_;
};

class ArgumentException : public Exception
{
public:
    explicit ArgumentException(core::StringView arg_name, core::StringView msg);
    explicit ArgumentException(core::StringView msg) : Exception(msg) {}
};
}   // namespace core
