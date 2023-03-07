#pragma once

namespace CE
{

    enum class RHIGraphicsBackend
    {
        None,
        Vulkan,
        Metal
    };

    enum class RHIResourceType
    {
        None,
        Buffer,

        RenderTarget,
        RenderPass,
        Viewport
    };

    enum class RHIDeviceObjectType
    {
        None,
        Buffer,
        Texture
    };

    enum RHIConstants
    {
        /// Max number of color render outputs per RHIRenderTarget
        RHIMaxSimultaneousRenderOutputs = 4,

        /// Max number of vertex attributes
        RHIMaxVertexAttribs = 8,

        /// Max number of resource groups (aka no. of descriptor sets for vulkan)
        RHIMaxPipelineResourceGroups = 4,

        /// Max number of subpasses in a single renderpass
        RHIMaxSubpasses = 4,
    };

    /// Render Target Color Format: Always prefer using Auto
    enum class RHIColorFormat
    {
        // Auto uses the same format as SwapChain. Always prefer using Auto over others.
        Auto,
        RGBA32,
        BGRA32,
    };

    /// Render Target Depth Format: Always prefer using Auto or None
    enum class RHIDepthStencilFormat
    {
        None,
        Auto,
        D32_S8,
        D24_S8,
        D32
    };

    enum class RHIRenderPassLoadAction : u8
    {
        // Contents are undefined and not preserved
        None,

        // Load existing content
        Load,

        // Clear existing content to a specified clear value
        Clear
    };

    enum class RHIRenderPassStoreAction : u8
    {
        // Contents are not stored in the memory, i.e. they are discarded. For ex: depth/stencil buffer content doesn't need to be stored
        None,

        // Contents of render target is stored to the memory
        Store,
    };
    
    struct RHIRenderTargetColorOutputDesc
    {
        RHIColorFormat preferredFormat{};
        RHIRenderPassLoadAction loadAction{};
        RHIRenderPassStoreAction storeAction{};
        u32 sampleCount = 1;
    };

    struct RHIRenderTargetLayout
    {
        u32 numColorOutputs = 0;
        RHIRenderTargetColorOutputDesc colorOutputs[RHIMaxSimultaneousRenderOutputs] = {};
        RHIDepthStencilFormat depthStencilFormat = RHIDepthStencilFormat::Auto;

        /// Index in renderOutputs[] of the render target that is going to be presented on screen.
        /// Or -1 if none are supposed to be consumed by SwapChain for presentation.
        s32 presentationRTIndex = -1;
    };

} // namespace CE
