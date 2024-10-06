/**
 * @file TickManager.h
 * @author Echo 
 * @Date 24-10-6
 * @brief 
 */

#pragma once
#include "CoreDef.h"
#include "Singleton/Singleton.h"
#include "Utils/ContainerUtils.h"

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

    void PerformTickLogic() const;

private:
    // 所有可Tick对象
    TArray<ITickable*> tickables_;
    TArray<ITickable*> tickable_game_objects_;
    TArray<ITickable*> tickable_components_;
};
}   // namespace function
