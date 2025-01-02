//
// Created by Echo on 25-1-2.
//

#pragma once
#include "Core/Utils/TimeUtils.h"


#include <chrono>

namespace func
{
class ITick
{
public:
    ITick();

    virtual ~ITick();
    /**
     * 设置Tick时间的缩放
     * @param scale
     */
    void SetTickScale(float scale);

    [[nodiscard]] float GetTickScale() const { return tick_scale_; }

    /**
     * 执行Tick的一系列函数, 传入数值是delta_time * scale
     */
    virtual void Tick(Millisecond delta_time) {}
    virtual void PreTick(Millisecond delta_time) {}
    virtual void PostTick(Millisecond delta_time) {}

    /**
     * 是否受具备时间缩放影响
     * @return
     */
    virtual bool IsTimeScaleInfluenced() const { return true; }

    /**
     * 是否受全局时间缩放影响
     * @return
     */
    virtual bool IsGlobalTimeScaleInfluenced() const { return true; }

protected:
    float tick_scale_ = 1.0f;
};
}   // namespace func
