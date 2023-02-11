#pragma once

#include "Messaging/MessageBus.h"

#include "RHIResources.h"

namespace CE
{

    class SYSTEM_API DynamicRHI
    {
    public:
        
        virtual ~DynamicRHI() = default;
        
        // Lifecycle
        virtual void Initialize() = 0;
        virtual void PostInitialize() = 0;
        virtual void PreShutdown() = 0;
        virtual void Shutdown() = 0;
        
        virtual void GetNativeHandle(void** outInstance) = 0;

        virtual RHIGraphicsBackend GetGraphicsBackend() = 0;
    };

    SYSTEM_API extern DynamicRHI* gDynamicRHI;
    
} // namespace CE
