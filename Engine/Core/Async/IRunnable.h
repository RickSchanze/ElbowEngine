//
// Created by Echo on 25-3-7.
//

#pragma once

namespace core {

class IRunnable {
public:
  virtual ~IRunnable() = default;

  // 返回true代表已经运行完成
  virtual bool Run() = 0;
};

} // namespace core
