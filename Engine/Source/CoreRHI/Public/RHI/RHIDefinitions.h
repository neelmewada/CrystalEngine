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
		ShaderResourceGroup,

		MemoryHeap,
        RenderTarget,
        RenderPass,
        Viewport,
		SwapChain,

		CommandQueue,
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

	enum class IndexFormat
	{
		Uint16 = 0,
		Uint32
	};

	ENUM(Flags)
    enum class BufferBindFlags
    {
        Undefined = 0,
        VertexBuffer = BIT(0),
        IndexBuffer = BIT(1),
        ConstantBuffer = BIT(2),
        StructuredBuffer = BIT(3),
        // Internal usage only!
        StagingBuffer = BIT(4),
    };
    ENUM_CLASS_FLAGS(BufferBindFlags);

    enum class BufferAllocMode
    {
        Default = 0,
        /// @brief Use memory shared with CPU & GPU
        SharedMemory,
        /// @brief Use only GPU visible memory
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
		//BufferAllocMode allocMode{};
		u64 bufferSize = 0;
		u64 structureByteStride = 0;

		const BufferData* initialData = nullptr;
	};

    /*
    *   Texture
    */

	ENUM()
    enum class Dimension
    {
        Dim2D = 0,
        Dim3D,
        Dim1D
    };

	ENUM()
    enum class Format
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
		D16_UNORM_S8_UINT,
		D24_UNORM_S8_UINT,
        D32_SFLOAT_S8_UINT,
		// Compressed formats
		BC7_UNORM,
		BC4_UNORM,
		BC6H_UFLOAT,
    };
	ENUM_CLASS(Format);

    typedef Format TextureFormat;

	ENUM(Flags)
    enum class TextureBindFlags
    {
		None = 0,
		/// @brief Use as shader input.
		ShaderRead = BIT(0),
		/// @brief Use as shader output.
		ShaderWrite = BIT(1),
		/// @brief Use with shader read & write access.
		ShaderReadWrite = ShaderRead | ShaderWrite,
		/// @brief Use as color attachment.
		Color = BIT(2),
		/// @brief Use as depth stencil attachment.
		DepthStencil = BIT(3),
		/// @brief Use as depth-only attachment.
		Depth = BIT(4),
		/// @brief Use as a subpass input.
		SubpassInput = BIT(5),
    };
    ENUM_CLASS_FLAGS(TextureBindFlags);

	ENUM()
	enum class FilterMode
	{
		Linear = 0,
		Nearest = 1,
		Cubic = 2
	};
	ENUM_CLASS(FilterMode);

    struct TextureDesc
    {
        Name name{};
        u32 width = 128, height = 128, depth = 1;
		Dimension dimension = Dimension::Dim2D;
        Format format{};
        u32 mipLevels = 1;
        u32 sampleCount = 1;
		TextureBindFlags bindFlags = TextureBindFlags::ShaderRead;
    };

	typedef TextureDesc ImageDesc;

    enum class SamplerAddressMode
    {
        Repeat = 0,
        MirroredRepeat = 1,
        ClampToEdge = 2,
        ClampToBorder = 3,
    };
    ENUM_CLASS(SamplerAddressMode);

    struct SamplerDescriptor
    {
        SamplerAddressMode addressModeU{};
        SamplerAddressMode addressModeV{};
        SamplerAddressMode addressModeW{};
		FilterMode samplerFilterMode{};
		Color borderColor{};
        b8 enableAnisotropy = false;
        int maxAnisotropy = 16;
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

	struct ViewportState
	{
		ViewportState() = default;

		ViewportState(float x, float y,
			float width, float height,
			float minDepth = 0.0f, float maxDepth = 1.0f) 
			: x(x), y(y) , width(width), height(height), minDepth(minDepth), maxDepth(maxDepth)
		{}

		float x = 0, y = 0;
		float width = 0, height = 0;
		float minDepth = 0.0f, maxDepth = 1.0f;
	};

	struct ScissorState
	{
		ScissorState() = default;

		ScissorState(float x, float y, float width, float height)
			: x(x), y(y), width(width), height(height)
		{}

		float x = 0, y = 0;
		float width = 0, height = 0;
	};

    /*
    *   Shader
    */

	class ShaderModule;

    enum class PipelineType
    {
        None = 0,
        Graphics,
        Compute
    };

	ENUM()
    enum class ShaderStage
    {
        None = 0,
        Vertex = BIT(0),
        Fragment = BIT(1),
		Tessellation = BIT(2),
		Geometry = BIT(3),

		Default = Vertex | Fragment,
		All = Vertex | Fragment | Tessellation | Geometry,
		COUNT = 4,
    };
    ENUM_CLASS_FLAGS(ShaderStage);

	struct ShaderModuleDescriptor
	{
		String name = "";
		ShaderStage stage = ShaderStage::None;
		void* byteCode = nullptr;
		SIZE_T byteSize = 0;
	};

	ENUM()
	enum class ShaderResourceType
	{
		None = 0,
		ConstantBuffer, // A uniform buffer in vulkan
		StructuredBuffer, // A storage buffer in vulkan
		RWStructuredBuffer, // A RW storage buffer in vulkan
		Texture1D, // A texture1D in vulkan
		Texture2D, // A texture2D in vulkan
		Texture3D, // A texture3D in vulkan
		TextureCube, // A textureCube in vulkan
		RWTexture2D, // An image2D in vulkan
		RWTexture3D, // An image3D in vulkan
		SamplerState, // A sampler in vulkan
		SubpassInput // A SubpassInput in vulkan
	};
	ENUM_CLASS(ShaderResourceType);

	ENUM()
	enum class CullMode
	{
		None = 0,
		Back,
		Front,
		All
	};
	ENUM_CLASS(CullMode);

} // namespace CE

#include "RHIDefinitions.rtti.h"
