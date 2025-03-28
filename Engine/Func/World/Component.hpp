//
// Created by Echo on 2025/3/25.
//
#pragma once
#include "Core/Object/Object.hpp"
#include "Core/Object/ObjectPtr.hpp"


class Actor;
class Component : public Object {
  REFLECTED_CLASS(Component)
public:
  Component();
  Component(const Component &) = delete;

  void SetOwner(const Actor *owner);

  Actor *GetOwner() const { return owner_; }

  void SetDirty();

  [[nodiscard]] bool IsDirty() const { return dirty_; }

protected:
  REFLECTED()
  ObjectPtr<Actor> owner_ = nullptr;

  bool dirty_ = false;
};