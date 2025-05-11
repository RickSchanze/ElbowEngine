#pragma once
#include "Core/Object/ObjectPtr.hpp"
#include "Resource/Assets/Asset.hpp"
#include "SceneMeta.hpp"

#include GEN_HEADER("Scene.generated.hpp")

class ECLASS(CustomSerialization) Scene : public Asset
{
    GENERATED_BODY(Scene)
public:
    virtual void PerformLoad() override;

    virtual void PerformUnload() override;

#if WITH_EDITOR
    virtual void Save() override;
#endif

private:
    Array<ObjectPtr<Object>> mTopObjects;
    SceneMeta mMeta;
};
