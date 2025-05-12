//
// Created by Echo on 2025/3/21.
//
#include "Field.hpp"

#include "ContainerView.hpp"
#include "Core/Logger/Logger.hpp"
#include "Core/Memory/New.hpp"
#include "Helper.hpp"

Field::~Field() { Delete(container_view); }

SequentialContainerView *Field::CreateSequentialContainerView(void *obj) const {
  if (!container_view) {
    Log(Error) << String::Format("类{}字段{}不是一个容器", *outer->GetName(), *name);
    return nullptr;
  }
  container_view->SetInstance(obj);
  return static_cast<SequentialContainerView *>(container_view);
}

AssociativeContainerView *Field::CreateAssociativeContainerView(void *obj) const {
  if (!container_view) {
    Log(Error) << String::Format("类{}字段{}不是一个容器", *outer->GetName(), *name);
    return nullptr;
  }
  container_view->SetInstance(obj);
  return static_cast<AssociativeContainerView *>(container_view);
}

bool Field::IsPrimitive() const { return type && NReflHelper::IsPrimitive(type); }
