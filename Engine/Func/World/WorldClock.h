//
// Created by Echo on 25-1-2.
//

#pragma once
#include "Core/Base/CoreTypeDef.h"
#include "Core/Event/Event.h"
#include "Core/Singleton/MManager.h"
#include "Core/Utils/TimeUtils.h"

namespace func {
class ITick;
}
namespace func {

// 超过33ms的帧被认为是异常帧, 被强制Clamp到33ms
constexpr auto TOO_LONG_FRAME_TIME = Millisecond(33);

DECLARE_MULTICAST_EVENT(OnMulticastTick, const Millisecond & /** 缩放后的 */);
DECLARE_EVENT(OnTick, void, const Millisecond & /** 缩放后的 */);

class TickEvents {
public:
  static inline OnTick RenderTickEvent;
  // InputTick: 用于给底层窗口库Tick, 例如GLFW就是glfwPollEvents
  static inline OnTick InputTickEvent;
  // 底层库Poll完成后, 输入数据获取, 此时针对此帧的输入进行处理, 例如上一帧A键松下, 这一帧放开就在这里分发Release时间
  static inline OnMulticastTick PostInputTickEvent;
  static inline OnMulticastTick WorldPreTickEvent;
  static inline OnMulticastTick WorldTickEvent;
  static inline OnMulticastTick WorldPostTickEvent;
};

class WorldClock : public core::Manager<WorldClock> {
public:
  void TickAll();

  [[nodiscard]] Millisecond GetDeltaTime() const;

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

  core::ManagerLevel GetLevel() const override { return core::ManagerLevel::L4; }
  core::StringView GetName() const override { return "WorldClock"; }

private:
  core::Array<ITick *> ticks_;

  float global_time_scale_ = 1.f;

  // 帧号
  uint64_t frame_counter_ = 0;

  // 上一次tick时间点, 用于获取delta_time
  std::chrono::steady_clock::time_point last_tick_tp_ = std::chrono::steady_clock::now();

  // 全局delta_time 不会被Clamp
  Millisecond delta_time_ = Millisecond(0);
};

WorldClock &GetWorldClock();

Millisecond GetDeltaTime();

} // namespace func
