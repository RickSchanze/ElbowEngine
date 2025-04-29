//
// Created by Echo on 2025/3/21.
//

#pragma once
#include "Core/Collection/Map.hpp"
#include "Core/String/String.hpp"
#include "Singleton.hpp"

class ManagerBase {
public:
    virtual ~ManagerBase() = default;

    virtual Float GetLevel() const = 0;
    virtual void Startup() {}
    virtual void Shutdown() {}
    virtual StringView GetName() const = 0;
};

// Manager of Manager
class MManager : public Singleton<MManager> {
public:
    MManager() = default;
    void RegisterManager(ManagerBase *manager);

    void Shutdown() const;

    void DumpManager();

private:
    Array<ManagerBase *> managers_ = {};
};

template<typename T>
class Manager : public ManagerBase {
public:
    static T *Get() {
        static T *instance;
        if (!instance) {
            instance = new T;
        }
        MManager::GetByRef().RegisterManager(instance);
        return instance;
    }

    static T &GetByRef() { return *Get(); }
};
