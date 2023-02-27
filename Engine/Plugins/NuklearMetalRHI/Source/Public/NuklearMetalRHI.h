#pragma once

#include "CoreMinimal.h"
#include "RHI/RHI.h"

namespace CE
{
    class MetalDevice;

    class NUKLEARMETALRHI_API NuklearMetalRHIModule : public PluginModule
    {
    public:
        NuklearMetalRHIModule() {}
        virtual ~NuklearMetalRHIModule() {}

        virtual void StartupModule() override;
        virtual void ShutdownModule() override;
        virtual void RegisterTypes() override;
        
    };

    class NUKLEARMETALRHI_API NuklearMetalRHI : public DynamicRHI
    {
    public:

        virtual void Initialize() override;
        virtual void PostInitialize() override;
        virtual void PreShutdown() override;
        virtual void Shutdown() override;
        
        virtual void GetNativeHandle(void** outInstance) override;
        
        virtual RHIGraphicsBackend GetGraphicsBackend() override;

    private:
        MetalDevice* device = nullptr;
    };
}
