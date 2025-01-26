//
// Created by Echo on 25-1-2.
//

#include "WorldClock.h"

#include "Core/Async/ThreadManager.h"
#include "Core/Math/Math.h"
#include "Core/Profiler/ProfileMacro.h"
#include "ITick.h"

#include <range/v3/all.hpp>

using namespace func;
using namespace core;
using namespace ranges;

void WorldClock::TickAll() {
  auto now = std::chrono::steady_clock::now();
  auto duration = std::chrono::duration_cast<Millisecond>(now - last_tick_tp_);
  last_tick_tp_ = now;
  delta_time_ = duration;
  duration = TimeUtils::Clamp(duration, Millisecond(0), Millisecond(1000));
  {
    PROFILE_SCOPE("Tick Main Thread Tasks");
    ThreadManager::Poll(10);
  }
  {
    PROFILE_SCOPE("TickInput");
    TickEvents::InputTickEvent.Invoke(duration);
  }
  {
    PROFILE_SCOPE("TickLogic");
    static Array<Millisecond> deltas;
    deltas.resize(ticks_.size());
    {
      {
        PROFILE_SCOPE("PreTick Tickable Object");
        for (int i = 0; i < ticks_.size(); ++i) {
          auto &tick = *ticks_[i];
          deltas[i] =
              duration * (tick.GetTickScale()) * (tick.IsGlobalTimeScaleInfluenced() ? global_time_scale_ : 1.f);
          ticks_[i]->PreTick(deltas[i]);
        }
      }
      {
        PROFILE_SCOPE("PreTick Other")
        TickEvents::WorldPreTickEvent.Invoke(duration);
      }
    }
    {
      {
        PROFILE_SCOPE("Tick Tickable Object");
        for (int i = 0; i < ticks_.size(); ++i) {
          ticks_[i]->Tick(deltas[i]);
        }
      }
      {
        PROFILE_SCOPE("Tick Other");
        TickEvents::WorldPostTickEvent.Invoke(duration);
      }
    }
    {
      {
        PROFILE_SCOPE("PostTick Tickable Object");
        for (int i = 0; i < ticks_.size(); ++i) {
          ticks_[i]->PostTick(deltas[i]);
        }
      }
      {
        PROFILE_SCOPE("PostTick Other")
        TickEvents::WorldPostTickEvent.Invoke(duration);
      }
    }
  }
  {
    PROFILE_SCOPE("TickRender");
    TickEvents::RenderTickEvent.Invoke(duration);
  }
}

Millisecond WorldClock::GetDeltaTime() const { return delta_time_; }

void WorldClock::SetGlobalTimeScale(float scale) { global_time_scale_ = Math::Clamp(scale, 0.f, scale); }

float WorldClock::GetGlobalTimeScale() const { return global_time_scale_; }

void WorldClock::RegisterTick(ITick *tick) {
  if (contains(ticks_, tick)) {
    return;
  }
  ticks_.push_back(tick);
}

void WorldClock::UnRegisterTick(ITick *tick) { ranges::remove(ticks_, tick); }

WorldClock &func::GetWorldClock() { return WorldClock::GetByRef(); }

Millisecond func::GetDeltaTime() { return GetWorldClock().GetDeltaTime(); }