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

    PROPERTY(Serialized)
    int32 mTestInt32 = 15;

    PROPERTY(Serialized)
    bool mTestBoolTrue = true;

    PROPERTY(Serialized)
    bool mTestBoolFalse = false;

    PROPERTY(Serialized)
    Array<float> mTestFloatArray{1.5, 2.1, 3.7, 4.6};

    PROPERTY(Serialized)
    StaticArray<double, 5> TestStaticDoubleArray{4.7, 9.5, 120.4, 61.7, 22.1};

    PROPERTY(Serialized)
    Map<int8, float> mTestSimpleMap{{1, 2.7}, {2, 3.9}, {3, 4.5}};

    PROPERTY(Serialized)
    Map<TestComplicatedKey, double> mTestComplicatedMap{
        {TestComplicatedKey(), 1.5}, {TestComplicatedKey(), 2.5}
    };

    PROPERTY(Serialized)
    Set<char> mTestSimpleSet = {'1', '2', '3'};

    PROPERTY(Serialized)
    Set<TestComplicatedKey> mTestComplicatedSet = {TestComplicatedKey(), TestComplicatedKey()};
};



#endif   //TESTSERIALIZATION_H
