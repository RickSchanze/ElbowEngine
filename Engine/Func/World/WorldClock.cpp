//
// Created by Echo on 2025/3/25.
//

#include "WorldClock.hpp"

#include "Core/Async/ThreadManager.hpp"
#include "Core/Math/Math.hpp"
#include "Core/Profile.hpp"
#include "ITick.hpp"
#include "Platform/Window/PlatformWindow.hpp"
#include "Platform/Window/PlatformWindowManager.hpp"


class ITick;
void WorldClock::TickAll(PlatformWindow *w) {
    const auto now = Now();
    auto duration = CastDuration<std::chrono::duration<Float, std::milli>>(now - last_tick_tp_);
    last_tick_tp_ = now;
    delta_time_ = duration;
    // duration = TimeUtils::Clamp(duration, Millisecond(0), Millisecond(1000));
    {
        ProfileScope _("Tick Main Thread Tasks");
        ThreadManager::PollGameThread(10);
    }
    {
        ProfileScope _("TickInput");
        TickEvents::Evt_TickInput.Invoke(duration);
        if (w->ShouldClose()) {
            return;
        }
        TickEvents::Evt_PostInputTick.Invoke(duration);
    }
    {
        ProfileScope _("TickLogic");
        static Array<MilliSeconds> deltas;
        deltas.Resize(ticks_.Count());
        {
            {
                ProfileScope _("PreTick Tickable Object");
                for (int i = 0; i < ticks_.Count(); ++i) {
                    auto &tick = *ticks_[i];
                    deltas[i] = duration * (tick.GetTickScale()) * (tick.IsGlobalTimeScaleInfluenced() ? global_time_scale_ : 1.f);
                    ticks_[i]->PreTick(deltas[i]);
                }
            }
            {
                ProfileScope _("PreTick Other");
                TickEvents::Evt_WorldPreTick.Invoke(duration);
            }
        }
        {
            {
                ProfileScope _("Tick Tickable Object");
                for (int i = 0; i < ticks_.Count(); ++i) {
                    ticks_[i]->Tick(deltas[i]);
                }
            }
            {
                ProfileScope _("Tick Other");
                TickEvents::Evt_WorldPostTick.Invoke(duration);
            }
        }
        {
            {
                ProfileScope _("PostTick Tickable Object");
                for (int i = 0; i < ticks_.Count(); ++i) {
                    ticks_[i]->PostTick(deltas[i]);
                }
            }
            {
                ProfileScope _("PostTick Other");
                TickEvents::Evt_WorldPostTick.Invoke(duration);
            }
        }
    }
    {
        ProfileScope _("TickRender");
        TickEvents::Evt_TickRender.Invoke(duration);
    }
}

MilliSeconds WorldClock::GetDeltaTime() const { return delta_time_; }

void WorldClock::SetGlobalTimeScale(float scale) { global_time_scale_ = Math::Clamp(scale, 0.f, scale); }

float WorldClock::GetGlobalTimeScale() const { return global_time_scale_; }

void WorldClock::RegisterTick(ITick *tick) {
    if (ticks_.Contains(tick)) {
        return;
    }
    ticks_.Add(tick);
}

void WorldClock::UnRegisterTick(ITick *tick) { ticks_.Remove(tick); }

WorldClock &GetWorldClock() { return WorldClock::GetByRef(); }

MilliSeconds GetDeltaTime() { return GetWorldClock().GetDeltaTime(); }
