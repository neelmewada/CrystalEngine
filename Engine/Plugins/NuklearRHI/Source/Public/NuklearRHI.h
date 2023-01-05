#pragma once

#include "RHI/RHI.h"

#include "Module/ModuleManager.h"

namespace CE
{

    NUKLEARRHI_API extern DynamicRHI* GDynamicRHI;

    class NUKLEARRHI_API NuklearRHIModule : public PluginModule
    {
    public:
        NuklearRHIModule() {}
        virtual ~NuklearRHIModule() {}
        
        virtual void StartupModule() override;
        virtual void ShutdownModule() override;
        
        void Initialize();
        void PreShutdown();
    };
    
} // namespace CE
