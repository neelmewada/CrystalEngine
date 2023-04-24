#pragma once

#include "Misc/CoreDefines.h"

namespace CE
{

    enum class ModuleType
    {
        Undefined,
        Module,
        PluginModule,
        GameModule
    };

    class CORE_API Module
    {
    public:
        Module() {}
        virtual ~Module() {}

        CE_NO_COPY(Module);

        virtual void StartupModule() = 0;
        virtual void ShutdownModule() = 0;

        virtual void RegisterTypes() {}
        virtual void DeregisterTypes() {}

        virtual bool IsPluginModule() { return false; }
        virtual bool IsGameModule() { return false; }
    };

    class CORE_API PluginModule : public Module
    {
    public:
        CE_NO_COPY(PluginModule);

        PluginModule() {}
        virtual ~PluginModule() {}
        
        virtual bool IsPluginModule() override { return true; }
    };

    class DefaultModuleImpl : public Module
    {
    public:

        void StartupModule() override
        {

        }

        void ShutdownModule() override
        {

        }

    };

    class DefaultPluginImpl : public PluginModule
    {
    public:

        void StartupModule() override
        {

        }

        void ShutdownModule() override
        {

        }

    };
    
} // namespace CE


