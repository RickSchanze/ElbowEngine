/**
 * @file Exception.h
 * @author Echo 
 * @Date 24-4-18
 * @brief 
 */

#pragma once
#include "Exception/Exception.h"

// TODO: 去Exception化
class VulkanException final : public Exception
{
public:
    explicit VulkanException(String InMessage) : Exception(std::move(InMessage)) {}

    [[nodiscard]] String What() const override;
};
