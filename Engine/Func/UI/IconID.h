//
// Created by Echo on 25-2-22.
//

#pragma once
#include "Core/Base/CoreTypeDef.h"
#include "Core/Base/EString.h"

#define GENERATE_ICON_ID(name)                                                                                         \
  static UInt64 name() {                                                                                               \
    static UInt64 id = core::StringView(#name).GetHashCode();                                                          \
    return id;                                                                                                         \
  }                                                                                                                    \
  static core::StringView _CONCAT(Name_, name)() { return #name; }

namespace func::ui {
// 使用SHA256 UTF-8字符串计算的哈希值
// 工具: https://www.lddgo.net/encrypt/hash
class IconID {
public:
  GENERATE_ICON_ID(Close);
  GENERATE_ICON_ID(Expanded);
  GENERATE_ICON_ID(Folded);
  GENERATE_ICON_ID(White);
};
} // namespace func::ui
