/**
 * @file Delegate.h
 * @author Echo
 * @Date 24-10-19
 * @brief
 */

#pragma once

#include "Core/Base/CoreTypeDef.h"
#include "Core/Log/CoreLogCategory.h"
#include "Core/Log/Logger.h"
#include "fmt/format.h"

namespace core {
struct DelegateID {
  explicit DelegateID(bool auto_id = false) {
    if (auto_id)
      id_ = s_id_counter++;
  }

  [[nodiscard]] uint16_t GetID() const { return id_; }
  [[nodiscard]] bool IsValid() const { return id_ != 0; }

  void Reset() { id_ = 0; }

  bool operator==(const DelegateID &other) const { return id_ == other.id_; }

  bool operator!=(const DelegateID &other) const { return id_ != other.id_; }

  void Swap(DelegateID &other) noexcept { ::std::swap(id_, other.id_); }
  void Swap(DelegateID &&other) noexcept { id_ = Move(other.id_); }

private:
  static inline uint16_t s_id_counter = 1;

  uint16_t id_ = 0;
};
} // namespace core

template <> struct std::hash<core::DelegateID> {
  std::size_t operator()(const core::DelegateID &id) const noexcept { return std::hash<uint16_t>{}(id.GetID()); }
};

template <> struct fmt::formatter<core::DelegateID> {
  constexpr auto parse(format_parse_context &ctx) -> decltype(ctx.begin()) { return ctx.end(); }

  template <typename FormatContext>
  auto format(const core::DelegateID &input, FormatContext &ctx) -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), "{}", input.GetID());
  }
};

namespace core {
// TDelegate是一个带Id的TFunction wrapper
template <typename ReturnT, typename... ArgumentTypes> struct Delegate {
  using ReturnType = ReturnT;

  explicit Delegate(Function<ReturnT(ArgumentTypes...)> &&function) : function_(Move(function)), id_(true) {}
  Delegate() : function_(), id_(false) {}

  Delegate(Delegate &&other) noexcept : function_(Move(other.function_)), id_(other.id_) { other.id_.Reset(); }
  Delegate(const Delegate &other) : function_(other.function_) {}

  Delegate &operator=(Delegate &&other) noexcept {
    if (this != &other) {
      function_ = Move(other.function_);
      id_ = other.id_;
      other.id_.Reset();
    }
    return *this;
  }

  bool operator==(const Delegate &other) const { return id_ == other.id_; }

  [[nodiscard]] DelegateID GetID() const { return id_; }

  void Unbind() {
    function_ = {};
    id_.Reset();
  }

  [[nodiscard]] bool IsValid() const { return id_.IsValid(); }
  [[nodiscard]] bool HasBound() const { return function_ != nullptr; }

  void Bind(Function<ReturnT(ArgumentTypes...)> &&function) {
    Assert::Require(logcat::Core_Event, !HasBound(), "Delegate is already bound, rebinding...");
    function_ = Move(function);
    id_ = Move(DelegateID(true));
  }

  ReturnT Invoke(ArgumentTypes &&...args) {
    if (HasBound()) {
      return function_(Forward<ArgumentTypes>(args)...);
    } else {
      LOGGER.Warn(logcat::Core_Event,
                  "Delegate {} is not bound, invoke failed. return {{}} if there needs a return value.", GetID());
      if constexpr (!std::is_same_v<ReturnT, void>) {
        return {};
      }
    }
  }

private:
  Function<ReturnT(ArgumentTypes...)> function_;
  DelegateID id_;
};
} // namespace core
