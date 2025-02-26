/**
 * @file Singleton.h
 * @author Echo
 * @Date 24-4-7
 * @brief
 */

#pragma once

template <typename T> class Singleton {
  friend T;

public:
  virtual ~Singleton() = default;

  static T *Get() {
    static T instance_;
    return &instance_;
  }

  static T &GetByRef() { return *Get(); }
};

template <typename T> class ConstSingleton {
  friend T;

public:
  virtual ~ConstSingleton() = default;

  static const T *Get() {
    static T instance_;
    return &instance_;
  }

  static const T &GetByRef() { return *Get(); }
};
