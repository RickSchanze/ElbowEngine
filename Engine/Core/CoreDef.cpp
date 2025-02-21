/**
 * @file CoreDef.cpp
 * @author Echo
 * @Date 24-11-19
 * @brief
 */

#include "CoreDef.h"

#include "Base/Exception.h"
#include "Log/CoreLogCategory.h"
#include "Log/Logger.h"

bool ValidateFeatureState() {
  if (WITH_EDITOR) {
    if (!ENABLE_IMGUI) {
      LOGGER.Error(logcat::Engine, "Feature WITH_EDITOR requires ENABLE_IMGUI enabled");
      return false;
    }
  }
  return true;
}

void NoEntry() { throw core::Exception("不应该进入这里"); }
