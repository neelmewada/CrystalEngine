#pragma once

#include "Messaging/MessageBus.h"

#include "RHIResources.h"

namespace CE
{
    class RHIRenderTarget;

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

        // *******************************************
        // - Public API -

        // - Render Target -

        virtual RHIRenderTarget* CreateRenderTarget(u32 backBufferCount, u32 simultaneousFrameDraws, 
            u32 width, u32 height, 
            const RHIRenderTargetLayout& rtLayout) = 0;

        virtual void DestroyRenderTarget(RHIRenderTarget* renderTarget) = 0;

        // - Resources -

        virtual RHIBuffer* CreateBuffer(const RHIBufferDesc& bufferDesc) = 0;
        virtual void DestroyBuffer(RHIBuffer* buffer) = 0;

    };

    SYSTEM_API extern DynamicRHI* gDynamicRHI;

    class SYSTEM_API RHIRenderPass : public RHIResource
    {
    protected:
        RHIRenderPass() : RHIResource(RHIResourceType::RenderPass)
        {}
    public:
        virtual ~RHIRenderPass() = default;

        // - Public API -

        

    };

    /// A render target that is drawn to by GPU. It is automatically created for you in case of Viewport.
    class SYSTEM_API RHIRenderTarget : public RHIResource
    {
    protected:
        RHIRenderTarget() : RHIResource(RHIResourceType::RenderTarget)
        {}
    public:
        virtual ~RHIRenderTarget() = default;

        // - Public API -

        virtual bool IsViewportRenderTarget() = 0;

        virtual RHIRenderPass* GetRenderPass() = 0;
    };

    /// A viewport used to draw & present from GPU. Usually only used in runtime builds than in editor.
    class SYSTEM_API RHIViewport : public RHIResource
    {
    protected:
        RHIViewport() : RHIResource(RHIResourceType::Viewport)
        {}
    public:
        virtual ~RHIViewport() = default;

        // - Public API

        virtual RHIRenderTarget* GetRenderTarget() = 0;

        virtual void SetClearColor(const Color& color) = 0;
    };

    
    
} // namespace CE
