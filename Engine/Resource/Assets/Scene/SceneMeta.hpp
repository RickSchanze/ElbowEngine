#pragma once

#include "Core/Core.hpp"

#include GEN_HEADER("SceneMeta.generated.hpp")

struct ESTRUCT(SQLTable = "Scene") SceneMeta
{
    GENERATED_BODY(SceneMeta)

    EFIELD(SQLAttr = "(PrimaryKey|AutoIncrement)")
    Int32 Id{};

    EFIELD()
    Int32 ObjectHandle{};

    EFIELD()
    String Path;
};
