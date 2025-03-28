//
// Created by Echo on 2025/3/25.
//

#pragma once
#include "Core/Event/Event.hpp"
#include "Core/Manager/MManager.hpp"

class ITick;
// 超过33ms的帧被认为是异常帧, 被强制Clamp到33ms
constexpr auto TOO_LONG_FRAME_TIME = MilliSeconds(33);

struct MulticastTickEvent : MulticastEvent<void, const MilliSeconds & /** 缩放后的 */> {};
struct TickEvent : Event<void, const MilliSeconds & /** 缩放后的 */> {};

class TickEvents {
public:
    static inline TickEvent Evt_TickRender;
    // InputTick: 用于给底层窗口库Tick, 例如GLFW就是glfwPollEvents
    static inline TickEvent Evt_TickInput;
    // 底层库Poll完成后, 输入数据获取, 此时针对此帧的输入进行处理, 例如上一帧A键松下, 这一帧放开就在这里分发Release时间
    static inline MulticastTickEvent Evt_PostInputTick;
    static inline MulticastTickEvent Evt_WorldPreTick;
    static inline MulticastTickEvent Evt_WorldTick;
    static inline MulticastTickEvent Evt_WorldPostTick;
};

class WorldClock : public Manager<WorldClock> {
public:
    void TickAll();

    [[nodiscard]] MilliSeconds GetDeltaTime() const;

    /**
     * 设置全局时间缩放
     * @param scale
     */
    void SetGlobalTimeScale(float scale);

    /**
     * 获取全局时间缩放
     * @return
     */
    [[nodiscard]] float GetGlobalTimeScale() const;

    void RegisterTick(ITick *tick);
    void UnRegisterTick(ITick *tick);

    Float GetLevel() const override { return 9; }
    StringView GetName() const override { return "WorldClock"; }

private:
    Array<ITick *> ticks_{};

    float global_time_scale_ = 1.f;

    // 帧号
    UInt64 frame_counter_ = 0;

    // 上一次tick时间点, 用于获取delta_time
    TimePoint last_tick_tp_ = Now();

    // 全局delta_time 不会被Clamp
    MilliSeconds delta_time_ = MilliSeconds(0);
};

WorldClock &GetWorldClock();

MilliSeconds GetDeltaTime();
