/**
 * @file ResourceConfig.h
 * @author Echo 
 * @Date 24-6-11
 * @brief 
 */

#pragma once
#include "CoreDef.h"
#include "CoreEvents.h"
#include "Path/Path.h"

struct RuntimeResoureConfig
{
    bool bInitialized = false;
    Path default_lack_texture_path;
};

struct ResourceConfig
{
    String DefaultLackTexturePath = L"Textures/NoTexture.png";
};

inline ResourceConfig       gStaticResourceConfig;
inline RuntimeResoureConfig gResourceConfig;

static void OnProjectPathSetHandler()
{
    gResourceConfig.default_lack_texture_path = Path(gStaticResourceConfig.DefaultLackTexturePath);
    gResourceConfig.bInitialized           = true;
}

struct ResourceConfigEventRegister
{
    ResourceConfigEventRegister() { OnProjectPathSet.Bind(OnProjectPathSetHandler); }
};

static inline ResourceConfigEventRegister Z_Register_Resource_Event;
