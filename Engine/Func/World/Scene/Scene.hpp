//
// Created by Echo on 2025/4/8.
//

#pragma once
#include "Core/Collection/Array.hpp"
#include "Core/Object/ObjectManager.hpp"


class Actor;
// TODO: 序列化枚重构下的临时方案: 单利Scene
class Scene : public Singleton<Scene> {
public:
    template<typename T>
    T *CreateActor() {
        T *actor = CreateNewObject<T>();
        actors_.Add(actor);
        return actor;
    }

    const Array<Actor *> &GetActors() const { return actors_; }

private:
    Array<Actor *> actors_;
};
