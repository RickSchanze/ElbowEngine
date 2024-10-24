/**
 * @file ISerializer.h
 * @author Echo 
 * @Date 24-10-24
 * @brief 
 */

#pragma once


class Archive;
class ISerializer
{
public:
    virtual ~ISerializer() = default;

    virtual void Serialize(Archive& ar) = 0;
};
