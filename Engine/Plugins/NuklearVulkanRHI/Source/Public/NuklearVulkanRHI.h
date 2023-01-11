#pragma once

#include "CoreMinimal.h"
#include "RHI/RHI.h"

namespace CE
{
    class NUKLEARVULKANRHI_API NuklearVulkanRHIModule : public PluginModule
    {
    public:
        NuklearVulkanRHIModule() {}
        virtual ~NuklearVulkanRHIModule() {}

        virtual void StartupModule() override;
        virtual void ShutdownModule() override;
        virtual void RegisterTypes() override;
        
    };

    class NUKLEARVULKANRHI_API NuklearVulkanRHI : public DynamicRHI
    {
    public:

        virtual void Initialize() override;
        virtual void PreShutdown() override;
        virtual void Shutdown() override;
        
    };
    
} // namespace CE
