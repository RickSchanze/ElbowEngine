/**
 * @file TickManager.h
 * @author Echo 
 * @Date 24-10-6
 * @brief 
 */

#pragma once
#include "Core/CoreDef.h"
#include "Core/Singleton/Singleton.h"
#include "Core/Utils/ContainerUtils.h"
#include "Core/CoreGlobal.h"

namespace function::comp
{
class Component;
}
namespace function
{
class GameObject;
}

namespace function
{
class ITickable;

enum class ETickStage
{
    PreTick,
    Tick,
    PostTick,
    Count, // 无效Tick阶段
};

class TickManager : public Singleton<TickManager>
{
public:
    template<typename T>
    void Add(T* tickable)
    {
        static_assert(std::is_base_of_v<ITickable, T>, "T must derived from ITickable!");
        if constexpr (std::is_base_of_v<GameObject, T>)
        {
            tickable_game_objects_.push_back(tickable);
        }
        else if constexpr (std::is_base_of_v<comp::Component, T>)
        {
            tickable_components_.push_back(tickable);
        }
        else
        {
            tickables_.push_back(tickable);
        }
    }

    template <typename T>
    void Remove(T* tickable)
    {
        static_assert(std::is_base_of_v<ITickable, T>, "T must derived from ITickable!");
        if constexpr (std::is_base_of_v<GameObject, T>)
        {
            ContainerUtils::Remove(tickable_game_objects_, tickable);
        }
        else if constexpr (std::is_base_of_v<comp::Component, T>)
        {
            ContainerUtils::Remove(tickable_components_, tickable);
        }
        else
        {
            ContainerUtils::Remove(tickables_, tickable);
        }
    }

    void PerformTickLogic();

    ETickStage GetTickStage() const;

private:
    // 所有可Tick对象
    Array<ITickable*> tickables_;
    Array<ITickable*> tickable_game_objects_;
    Array<ITickable*> tickable_components_;

    ETickStage tick_stage_ = ETickStage::Count;
};
}   // namespace function

template<>
constexpr const char* GetEnumString<function::ETickStage>(function::ETickStage stage)
{
    switch (stage)
    {
    case function::ETickStage::PreTick: return "PreTick";
    case function::ETickStage::Tick: return "Tick";
    case function::ETickStage::PostTick: return "PostTick";
    }
    return "OutOfRange";
}
