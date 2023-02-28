#pragma once

#include "CoreMinimal.h"

#include "Application/Application.h"

// Configs
#include "Config/ConfigBase.h"
#include "Config/PluginConfig.h"
#include "Config/ConfigManager.h"

#include "Plugin/PluginManager.h"

// RHI
#include "RHI/RHI.h"

// Game Framework
#include "GameFramework/GameFramework.h"

namespace CE
{

    class SYSTEM_API SystemModule : public Module
    {
    public:

        virtual void StartupModule() override;

        virtual void ShutdownModule() override;

        virtual void RegisterTypes() override;

    };
    
} // namespace CE

