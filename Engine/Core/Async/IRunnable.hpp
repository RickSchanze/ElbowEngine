//
// Created by Echo on 2025/3/22.
//

#pragma once

class IRunnable {
public:
  virtual ~IRunnable() = default;

  // 返回true代表已经运行完成
  virtual bool Run() = 0;
};