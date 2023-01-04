#pragma once

#include "Misc/CoreDefines.h"

namespace CE
{

    class CORE_API Module
    {
    public:
        Module();
        virtual ~Module();

        CE_NO_COPY(Module);

        virtual void StartupModule() = 0;
        virtual void ShutdownModule() = 0;

        virtual void RegisterTypes() {}
    };

    class CORE_API PluginModule : public Module
    {
    public:
        CE_NO_COPY(PluginModule);

        PluginModule() {}
        virtual ~PluginModule() {}
        
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


