/**
 * @file TestSerialization.h
 * @author Echo 
 * @Date 2024/4/10
 * @brief 
 */

#ifndef TESTSERIALIZATION_H
#define TESTSERIALIZATION_H

#include "Core/Object/Object.h"
#include "TestSerialization.generated.h"

#include <iostream>

struct REFL TestComplicatedKey
{
    GENERATED_BODY(TestComplicatedKey)
    PROPERTY(Serialized)
    int32 mKey1 = 1;

    PROPERTY(Serialized)
    String mKey2 = L"键类型";

    PROPERTY(Serialized)
    AnsiString mKey3 = "Ansi key type";

    PROPERTY(Serialized)
    Array<String> mKey4 = {L"键组1", L"键组2", L"键组3"};

    PROPERTY(Serialized)
    Array<AnsiString> mKey5 = {"Ansi key group 1", "Ansi key group 2", "Ansi key group 3"};

    PROPERTY(Serialized)
    Map<String, int> mKey6 = {{L"键1", 1}, {L"键2", 2}, {L"键3", 3}};

public:
    bool operator<(const TestComplicatedKey& Other) const { return mKey1 < Other.mKey1; }
};

class REFL TestSerialization : public Object {
    GENERATED_BODY(TestSerialization)
public:

    PROPERTY(Serialized)
    int32 mTestInt32{};

    PROPERTY(Serialized)
    bool mTestBoolTrue{};

    PROPERTY(Serialized)
    bool mTestBoolFalse{};

    PROPERTY(Serialized)
    Array<float> mTestFloatArray{};

    PROPERTY(Serialized)
    StaticArray<double, 5> TestStaticDoubleArray{};

    PROPERTY(Serialized)
    Map<int8, float> mTestSimpleMap{};

    PROPERTY(Serialized)
    Map<TestComplicatedKey, TestComplicatedKey> mTestComplicatedMap{
    };

    PROPERTY(Serialized)
    Set<int8> mTestSimpleSet = {};

    PROPERTY(Serialized)
    Set<TestComplicatedKey> mTestComplicatedSet = {};
};



#endif   //TESTSERIALIZATION_H
