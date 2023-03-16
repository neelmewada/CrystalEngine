#pragma once

#include "Messaging/MessageBus.h"

#include "RHIResources.h"

namespace CE
{
    class SYSTEM_API RHIRenderTarget;
    class SYSTEM_API RHIViewport;
    class SYSTEM_API RHIRenderPass;
    class SYSTEM_API RHICommandList;
    class SYSTEM_API RHIGraphicsCommandList;

    class SYSTEM_API DynamicRHI
    {
    public:
        
        virtual ~DynamicRHI() = default;
        
        // Lifecycle
        virtual void Initialize() = 0;
        virtual void PostInitialize() = 0;
        virtual void PreShutdown() = 0;
        virtual void Shutdown() = 0;
        
        virtual void* GetNativeHandle() = 0;

        virtual RHIGraphicsBackend GetGraphicsBackend() = 0;

        // *******************************************
        // - Public API -

        // - Render Target -

        virtual RHIRenderTarget* CreateRenderTarget(u32 width, u32 height, 
            const RHIRenderTargetLayout& rtLayout) = 0;

        virtual void DestroyRenderTarget(RHIRenderTarget* renderTarget) = 0;

        virtual RHIViewport* CreateViewport(void* windowHandle, 
            u32 width, u32 height, bool isFullscreen,
            const RHIRenderTargetLayout& rtLayout) = 0;

        virtual void DestroyViewport(RHIViewport* viewport) = 0;

        // - Command List -

        virtual RHIGraphicsCommandList* CreateGraphicsCommandList(RHIViewport* viewport) = 0;
        virtual RHIGraphicsCommandList* CreateGraphicsCommandList(RHIRenderTarget* renderTarget) = 0;
        virtual void DestroyCommandList(RHICommandList* commandList) = 0;

        virtual bool ExecuteCommandList(RHICommandList* commandList) = 0;

        virtual bool PresentViewport(RHIGraphicsCommandList* viewportCommandList) = 0;

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

        virtual void SetClearColor(u32 colorTargetIndex, const Color& color) = 0;
    };

    /// A viewport used to draw & present from GPU. Usually only used in runtime builds than in editor.
    class SYSTEM_API RHIViewport : public RHIResource
    {
    protected:
        RHIViewport() : RHIResource(RHIResourceType::Viewport)
        {}
    public:
        virtual ~RHIViewport() = default;

        // - Public API -

        virtual RHIRenderTarget* GetRenderTarget() = 0;

        virtual void SetClearColor(const Color& color) = 0;

        virtual void Rebuild() = 0;
    };

    /*
    *   RHI Command List
    */

    class SYSTEM_API RHICommandList : public RHIResource
    {
    protected:
        RHICommandList() : RHIResource(RHIResourceType::CommandList)
        {}
    public:
        virtual ~RHICommandList() = default;

        // - Public API -

        virtual RHICommandListType GetCommandListType() = 0;



    };

    class SYSTEM_API RHIGraphicsCommandList : public RHICommandList
    {
    protected:
        RHIGraphicsCommandList() : RHICommandList()
        {}

    public:
        virtual ~RHIGraphicsCommandList() = default;

        // - Public API -

        virtual RHICommandListType GetCommandListType() override final { return RHICommandListType::Graphics; }

        // - Command List API -

        virtual void Begin() = 0;
        virtual void End() = 0;
    };
    
} // namespace CE
