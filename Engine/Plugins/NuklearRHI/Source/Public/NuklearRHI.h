#pragma once

#include "CoreMinimal.h"
#include "RHI/RHI.h"

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
        virtual void RegisterTypes() override;
        
        void Initialize();
        void PreShutdown();
    };
    
} // namespace CE
