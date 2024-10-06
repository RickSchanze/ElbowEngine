/**
 * @file TickManager.cpp
 * @author Echo 
 * @Date 24-10-6
 * @brief 
 */

#include "TickManager.h"

#include "ITickable.h"

namespace function
{

static void TickPreTick(const TArray<ITickable*>& objects_, const TArray<ITickable*>& comps, const TArray<ITickable*>& others)
{
    for (auto& comp : comps)
    {
        comp->PreTick();
    }
    for (auto& obj : objects_)
    {
        obj->PreTick();
    }
    for (auto& other : others)
    {
        other->PreTick();
    }
}

static void TickTick(const TArray<ITickable*>& objects_, const TArray<ITickable*>& comps, const TArray<ITickable*>& others)
{
    for (auto& comp : comps)
    {
        comp->Tick();
    }
    for (auto& obj : objects_)
    {
        obj->Tick();
    }
    for (auto& other : others)
    {
        other->Tick();
    }
}

static void TickPostTick(const TArray<ITickable*>& objects_, const TArray<ITickable*>& comps, const TArray<ITickable*>& others)
{
    for (auto& comp : comps)
    {
        comp->PostTick();
    }
    for (auto& obj : objects_)
    {
        obj->PostTick();
    }
    for (auto& other : others)
    {
        other->PostTick();
    }
}

void TickManager::PerformTickLogic() const
{
    // PreTick
    TickPreTick(tickable_game_objects_, tickable_components_, tickables_);
    TickTick(tickable_game_objects_, tickable_components_, tickables_);
    TickPostTick(tickable_game_objects_, tickable_components_, tickables_);
}

}
