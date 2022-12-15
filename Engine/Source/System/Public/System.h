#pragma once

#include "Module/ModuleManager.h"
#include "Application/Application.h"

#include "GameFramework/GameFramework.h"

namespace CE
{
    extern SYSTEM_API Engine* GEngine;

    class SYSTEM_API SystemModule : public Module
    {
    public:

        virtual void StartupModule() override;

        virtual void ShutdownModule() override;

        virtual void RegisterTypes() override;

    };
    
} // namespace CE

