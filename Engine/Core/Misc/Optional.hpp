//
// Created by Echo on 2025/3/21.
//

#pragma once
#include <optional>

template <typename T> class Optional {
public:
  Optional() = default;
  Optional(T &&value) : m_opt(std::forward<T>(value)) {}
  Optional(const T &value) : m_opt(value) {}
  Optional(const Optional &other) : m_opt(other.m_opt) {}
  Optional(Optional &&other) noexcept : m_opt(std::move(other.m_opt)) {}
  ~Optional() = default;

  Optional &operator=(const Optional &other) {
    if (this == &other) {
      return *this;
    }
    m_opt = other.m_opt;
    return *this;
  }

  Optional &operator=(Optional &&other) noexcept {
    if (this == &other) {
      return *this;
    }
    m_opt = std::move(other.m_opt);
    return *this;
  }

  Optional &operator=(const T &value) {
    m_opt = value;
    return *this;
  }

  Optional &operator=(T &&value) {
    m_opt = std::forward<T>(value);
    return *this;
  }

  operator bool() const { return m_opt.has_value(); }

  T &operator*() { return m_opt.value(); }
  const T &operator*() const { return m_opt.value(); }

  void Reset() { m_opt.reset(); }

private:
  std::optional<T> m_opt = std::nullopt;
};

template <typename T> Optional<T> MakeOptional(const T &val) { return Optional<T>(val); }