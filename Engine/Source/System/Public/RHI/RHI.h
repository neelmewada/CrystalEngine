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

    /// The target used as a rendering output. Could be either a Window or an offscreen target
    class SYSTEM_API RHIRenderTarget : public RHIResource
    {
    protected:
        RHIRenderTarget() : RHIResource(RHIResourceType::RenderTarget)
        {}
    public:
        virtual ~RHIRenderTarget() = default;

        virtual RHIRenderTargetType GetRenderTargetType() = 0;

    };
    
} // namespace CE
