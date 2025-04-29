//
// Created by Echo on 2025/3/25.
//

#pragma once
#include "Core/TypeAlias.hpp"

class ITick {
public:
    ITick();

    virtual ~ITick();
    /**
     * 设置Tick时间的缩放
     * @param scale
     */
    void SetTickScale(float scale);

    float GetTickScale() const { return tick_scale_; }

    /**
     * 执行Tick的一系列函数, 传入数值是delta_time * scale
     */
    virtual void Tick(MilliSeconds delta_time) {}
    virtual void PreTick(MilliSeconds delta_time) {}
    virtual void PostTick(MilliSeconds delta_time) {}

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
    Float tick_scale_ = 1.0f;
};
