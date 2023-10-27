#pragma once

#include "ImGui/ImGuiDefinitions.h"

namespace CE::RHI
{

	/// Only Vulkan is supported
    enum class GraphicsBackend
    {
        None,
        Vulkan,
    };

    enum class ResourceType
    {
        None,
        Buffer,
        Texture,
        Sampler,
        ShaderModule,
        ComputePipelineState,
        GraphicsPipelineState,

        RenderTarget,
        RenderPass,
        Viewport,

        CommandList
    };

    enum class DeviceObjectType
    {
        None,
        Buffer,
        Texture
    };
    
    enum Constants
    {
        /// Max number of color render outputs per RenderTarget
        MaxSimultaneousRenderOutputs = 4,

        /// Max number of vertex attributes
        MaxVertexAttribs = 8,

        /// Max number of shader resource groups (aka no. of descriptor sets for vulkan)
        MaxShaderResourceGroups = 4,

        /// Max number of subpasses in a single renderpass
        MaxSubpasses = 4,
    };

    /// Render Target Color Format: Always prefer using Auto
    enum class ColorFormat
    {
        // Auto uses the same format as SwapChain. Always prefer using Auto over others.
        Auto,
        RGBA32,
        BGRA32,
    };

    /// Render Target Depth Format: Always prefer using Auto or None
    enum class DepthStencilFormat
    {
        None,
        Auto,
        D32_S8,
        D24_S8,
        D32
    };

    enum class RenderPassLoadAction : u8
    {
        // Contents are undefined and not preserved
        None,

        // Load existing content
        Load,

        // Clear existing content to a specified clear value
        Clear
    };

    enum class RenderPassStoreAction : u8
    {
        // Contents are not stored in the memory, i.e. they are discarded. For ex: depth/stencil buffer content doesn't need to be stored
        None,

        // Contents of render target is stored to the memory
        Store,
    };
    
    struct RenderTargetColorOutputDesc
    {
        ColorFormat preferredFormat{};
        RenderPassLoadAction loadAction{};
        RenderPassStoreAction storeAction{};
        u32 sampleCount = 1;
    };

    struct RenderTargetLayout
    {
        u32 numColorOutputs = 0;
        RenderTargetColorOutputDesc colorOutputs[RHI::MaxSimultaneousRenderOutputs] = {};
        DepthStencilFormat depthStencilFormat = DepthStencilFormat::Auto;

        /// Index in renderOutputs[] of the render target that is going to be presented on screen.
        /// Or -1 if none are supposed to be consumed by SwapChain for presentation.
        s32 presentationRTIndex = -1;

        u32 backBufferCount = 2, simultaneousFrameDraws = 1;
    };

    struct FontDesc
    {
        SIZE_T byteSize = 0;
        u32 pointSize = 14;
        String fontName = "";
		b8 isCompressed = false;

        void* fontData = nullptr;
    };

    struct FontPreloadConfig
    {
        u32 preloadFontCount = 0;
        FontDesc* preloadFonts = nullptr;
    };

    /*
    *   Buffer
    */

    enum class BufferBindFlags
    {
        Undefined = 0,
        VertexBuffer = BIT(0),
        IndexBuffer = BIT(1),
        UniformBuffer = BIT(2),
        StorageBuffer = BIT(3),
        // Internal usage only!
        StagingBuffer = BIT(4),
    };

    ENUM_CLASS_FLAGS(BufferBindFlags);

    /// Features used by the buffer
    enum class BufferUsageFlags
    {
        Default = 0,
        /// Buffer can be used with dynamic offset
        DynamicOffset = BIT(0),
        // TODO: Not implemented yet (Buffer can be resized dynamically)
        DynamicSize = BIT(1),
    };
    ENUM_CLASS_FLAGS(BufferUsageFlags);

    enum class BufferAllocMode
    {
        Default = 0,
        SharedMemory,
        GpuMemory,
    };
    ENUM_CLASS_FLAGS(BufferAllocMode);

    struct BufferData
    {
        u64 dataSize;
        u64 startOffsetInBuffer;
        const void* data = nullptr;
    };

    struct BufferDesc
    {
        Name name{};
        BufferBindFlags bindFlags{};
        BufferUsageFlags usageFlags{};
        BufferAllocMode allocMode{};
        u64 bufferSize = 0;
        u64 structureByteStride = 0;

        const BufferData* initialData = nullptr;
    };

    /*
    *   Texture
    */

    enum class TextureDimension
    {
        Dim2D = 0,
        Dim3D,
        Dim1D,
        DimCUBE,
    };

    enum class TextureFormat
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
        B8G8R8_UNORM,
        B8G8R8_SNORM,
        B8G8R8_SRGB,
        R16_UNORM,
        R16_SNORM,
        R16_SFLOAT,
		R16G16_UNORM,
		R16G16_SNORM,
		R16G16_UINT,
		R16G16_SINT,
		R16G16_SFLOAT,
        R32_UINT,
        R32_SINT,
        R32_SFLOAT,
        D32_SFLOAT,
		R32G32_UINT,
		R32G32_SINT,
		R32G32_SFLOAT,
        D24_UNORM_S8_UINT,
        D32_SFLOAT_S8_UINT,
		// Compressed formats
		BC7_UNORM,
		BC4_UNORM,
		BC6H_UFLOAT,
    };

    enum class TextureUsageFlags
    {
		Default,
        SampledImage = BIT(0),
        ColorAttachment = BIT(1),
        DepthStencilAttachment = BIT(2),
    };
    ENUM_CLASS_FLAGS(TextureUsageFlags);

	enum FilterMode
	{
		FILTER_MODE_LINEAR = 0,
		FILTER_MODE_NEAREST = 1,
		FILTER_MODE_CUBIC = 2
	};
	ENUM_CLASS_FLAGS(FilterMode);

    struct TextureDesc
    {
        String name{};
        u32 width = 128, height = 128, depth = 1;
        TextureDimension dimension = TextureDimension::Dim2D;
        TextureFormat format{};
        u32 mipLevels = 1;
        u32 sampleCount = 1;
        TextureUsageFlags usageFlags = TextureUsageFlags::Default;

		/// Force linear tiling rather than optimal tiling
        bool forceLinearLayout = false;
    };

    enum SamplerAddressMode
    {
        SAMPLER_ADDRESS_MODE_REPEAT = 0,
        SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT = 1,
        SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE = 2,
        SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER = 3,
    };
    ENUM_CLASS_FLAGS(SamplerAddressMode);

    enum SamplerFilterMode
    {
        SAMPLER_FILTER_LINEAR = 0,
        SAMPLER_FILTER_NEAREST = 1,
    };

    struct SamplerDesc
    {
        SamplerAddressMode addressModeU{};
        SamplerAddressMode addressModeV{};
        SamplerAddressMode addressModeW{};
		FilterMode samplerFilterMode{};
        b8 enableAnisotropy = false;
        int maxAnisotropy = 16;
        Color borderColor{};
    };

    /*
    *   Command List
    */

    enum class CommandListType
    {
        None = 0,
        Graphics,
        Compute,
    };

    /*
    *   Shader
    */

    enum class PipelineType
    {
        None = 0,
        Graphics,
        Compute
    };

    enum class ShaderStage
    {
        None = 0,
        Vertex = BIT(0),
        Fragment = BIT(1),
    };
    ENUM_CLASS_FLAGS(ShaderStage);

	struct ShaderModuleDesc
	{
		String name = "";
		ShaderStage stage = ShaderStage::None;

	};

    /*
    *   Graphics Pipeline
    */

	struct GraphicsPipelineDesc
	{
		u32 vertexSizeInBytes = 0;
	};

} // namespace CE
