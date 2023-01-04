#pragma once

#include "RHI/RHI.h"

#include "Module/ModuleManager.h"

namespace CE
{

    class NUKLEARRHI_API NuklearRHI : public DynamicRHI
    {
    public:
        
    };

    class NUKLEARRHI_API NuklearRHIModule : public PluginModule
    {
    public:
        NuklearRHIModule() {}
        virtual ~NuklearRHIModule() {}
        
        virtual void StartupModule() override
        {
            
        }
        
        virtual void ShutdownModule() override
        {
            
        }
        
    };
    
} // namespace CE
