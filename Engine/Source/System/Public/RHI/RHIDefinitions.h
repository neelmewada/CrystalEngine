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
        Texture,

        RenderTarget,
        RenderPass,
        Viewport,

        CommandList
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

        u32 backBufferCount = 2, simultaneousFrameDraws = 1;
    };

    /*
    *   Buffer
    */

    enum class RHIBufferBindFlags
    {
        Undefined = 0,
        VertexBuffer = BIT(0),
        IndexBuffer = BIT(1),
        UniformBuffer = BIT(2),
        StorageBuffer = BIT(3),
        // Internal usage only!
        StagingBuffer = BIT(4),
    };

    ENUM_CLASS_FLAGS(RHIBufferBindFlags);

    /// Features used by the buffer
    enum class RHIBufferUsageFlags
    {
        Default = 0,
        /// Buffer can be used with dynamic offset
        DynamicOffset = BIT(0),
        // TODO: Not implemented yet (Buffer can be resized dynamically)
        DynamicSize = BIT(1),
    };
    ENUM_CLASS_FLAGS(RHIBufferUsageFlags);

    enum class RHIBufferAllocMode
    {
        Default = 0,
        SharedMemory,
        GpuMemory,
    };
    ENUM_CLASS_FLAGS(RHIBufferAllocMode);

    struct RHIBufferData
    {
        u64 dataSize;
        u64 startOffsetInBuffer;
        const void* data = nullptr;
    };

    struct RHIBufferDesc
    {
        Name name{};
        RHIBufferBindFlags bindFlags{};
        RHIBufferUsageFlags usageFlags{};
        RHIBufferAllocMode allocMode{};
        u64 bufferSize = 0;
        u64 structureByteStride = 0;

        const RHIBufferData* initialData = nullptr;
    };

    /*
    *   Texture
    */

    enum class RHITextureDimension
    {
        Dim2D = 0,
        Dim3D,
        Dim1D,
        DimCUBE,
    };

    enum class RHITextureFormat
    {
        Undefined = 0,
        R8_UNORM,
        R8_SNORM,
        R8_SRGB,
        R8G8B8A8_SRGB,
        R8G8B8A8_UNORM,
        R8G8B8A8_SNORM,
        B8G8R8A8_SRGB,
        B8G8R8A8_UNORM,
        B8G8R8A8_SNORM,
        R8G8B8_UNORM,
        R8G8B8_SNORM,
        R8G8B8_SRGB,
        R16_UNORM,
        R16_SNORM,
        R16_SFLOAT,
        R32_UINT,
        R32_SINT,
        R32_SFLOAT,
        D32_SFLOAT,
        D24_UNORM_S8_UINT,
        D32_SFLOAT_S8_UINT,
    };

    enum class RHITextureUsageFlags
    {
        SampledImage = BIT(0),
        ColorAttachment = BIT(1),
        DepthStencilAttachment = BIT(2),
        // For internal use only!
        Staging = BIT(3),

        Default = SampledImage,
    };
    ENUM_CLASS_FLAGS(RHITextureUsageFlags);

    struct RHITextureDesc
    {
        Name name{};
        u32 width = 128, height = 128, depth = 1;
        RHITextureDimension dimension = RHITextureDimension::Dim2D;
        RHITextureFormat format{};
        u32 mipLevels = 1;
        u32 sampleCount = 1;
        RHITextureUsageFlags usageFlags = RHITextureUsageFlags::Default;

        bool forceLinearLayout = false;
    };

    /*
    *   Command List
    */

    enum class RHICommandListType
    {
        None = 0,
        Graphics,
        Compute,
    };

} // namespace CE
