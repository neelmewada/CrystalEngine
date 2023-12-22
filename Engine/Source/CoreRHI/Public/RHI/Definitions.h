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
		ShaderResourceGroupManager,

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

	enum class IndexFormat
	{
		Uint16 = 0,
		Uint32
	};

    enum class BufferBindFlags
    {
        Undefined = 0,
        VertexBuffer = BIT(0),
        IndexBuffer = BIT(1),
        ConstantBuffer = BIT(2),
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
    enum class TextureUsageFlags
    {
		Default,
        SampledImage = BIT(0),
        ColorAttachment = BIT(1),
        DepthStencilAttachment = BIT(2),
		InputAttachment = BIT(3)
    };
    ENUM_CLASS_FLAGS(TextureUsageFlags);

	ENUM()
	enum class FilterMode
	{
		Linear = 0,
		Nearest = 1,
		Cubic = 2
	};
	ENUM_CLASS_FLAGS(FilterMode);


    struct TextureDesc
    {
        Name name{};
        u32 width = 128, height = 128, depth = 1;
        TextureDimension dimension = TextureDimension::Dim2D;
        Format format{};
        u32 mipLevels = 1;
        u32 sampleCount = 1;
        TextureUsageFlags usageFlags = TextureUsageFlags::Default;

		/// Force linear tiling rather than optimal tiling
        bool forceLinearLayout = false;
    };

	typedef TextureDesc ImageDesc;

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
		Tessellation = BIT(2),

		COUNT = 3,
    };
    ENUM_CLASS_FLAGS(ShaderStage);

	struct ShaderModuleDesc
	{
		String name = "";
		ShaderStage stage = ShaderStage::None;
		void* byteCode = nullptr;
		SIZE_T byteSize = 0;
	};

    /*
    *   Graphics Pipeline
    */

	class ShaderModule;
	class ShaderResourceGroup;

	enum ShaderResourceType
	{
		SHADER_RESOURCE_TYPE_NONE = 0,
		SHADER_RESOURCE_TYPE_CONSTANT_BUFFER,
		SHADER_RESOURCE_TYPE_STRUCTURED_BUFFER,
		SHADER_RESOURCE_TYPE_SAMPLED_IMAGE,
		SHADER_RESOURCE_TYPE_SAMPLER_STATE,
		SHADER_RESOURCE_TYPE_INPUT_ATTACHMENT
	};

	struct ShaderResourceVariableDesc
	{
		//u32 bindingSlot = 0;
		Name name = "";
		u32 arrayCount = 1;
		ShaderResourceType type = SHADER_RESOURCE_TYPE_NONE;
		b8 isDynamic = false;

		ShaderStage stageFlags = ShaderStage::Vertex | ShaderStage::Fragment;
	};

	ENUM()
	enum class SRGType
	{
		PerScene = 0,
		PerView,
		PerPass,
		PerSubPass,
		PerMaterial,
		PerObject,
		PerDraw,
        COUNT
	};
	ENUM_CLASS(SRGType);

	struct ShaderResourceGroupDesc
	{
		SRGType srgType = SRGType::PerScene;
		Name srgName = "PerScene"; // Ex: PerView
		Array<ShaderResourceVariableDesc> variables{};
	};

	struct ShaderStageDesc
	{
		RHI::ShaderModule* shaderModule = nullptr;
		RHI::ShaderStage stage = RHI::ShaderStage::None;
		String entry = "";
	};

	struct VertexAttribDesc
	{
		u32 location = 0;
		TypeId dataType = 0;
		u32 offset = 0;
	};

	enum CullMode
	{
		CULL_MODE_NONE = 0,
		CULL_MODE_BACK,
		CULL_MODE_FRONT,
		CULL_MODE_ALL
	};

	struct GraphicsPipelineDesc
	{
		u32 vertexSizeInBytes = 0;
		Array<VertexAttribDesc> vertexAttribs{};

		RHI::ShaderModule* vertexShader = nullptr;
		String vertexEntry = "VertMain";
		RHI::ShaderModule* fragmentShader = nullptr;
		String fragmentEntry = "FragMain";
		Array<ShaderStageDesc> otherStages{};

		CullMode cullMode = CULL_MODE_BACK;

		Array<RHI::ShaderResourceGroupDesc> shaderResourceGroups{};

		HashMap<Name, int> variableNameBindingMap{};
	};

	struct CORERHI_API GraphicsPipelineBuilder
	{
		GraphicsPipelineBuilder() {}

		GraphicsPipelineBuilder& VertexSize(u32 byteSize)
		{
			desc.vertexSizeInBytes = byteSize;
			return *this;
		}

		GraphicsPipelineBuilder& VertexAttrib(u32 location, TypeId dataType, u32 offset)
		{
			VertexAttribDesc attrib{};
			attrib.location = location;
			attrib.dataType = dataType;
			attrib.offset = offset;
			desc.vertexAttribs.Add(attrib);
			return *this;
		}

		GraphicsPipelineBuilder& VertexShader(RHI::ShaderModule* shaderModule, const String& entry = "VertMain")
		{
			desc.vertexShader = shaderModule;
			desc.vertexEntry = entry;
			return *this;
		}

		GraphicsPipelineBuilder& FragmentShader(RHI::ShaderModule* shaderModule, const String& entry = "FragMain")
		{
			desc.fragmentShader = shaderModule;
			desc.fragmentEntry = entry;
			return *this;
		}

		GraphicsPipelineBuilder& CullMode(CullMode cullMode)
		{
			desc.cullMode = cullMode;
			return *this;
		}

		GraphicsPipelineBuilder& SRG(const RHI::ShaderResourceGroupDesc& srg)
		{
			desc.shaderResourceGroups.Add(srg);
			return *this;
		}

		const GraphicsPipelineDesc& Build()
		{
			return desc;
		}

	private:

		GraphicsPipelineDesc desc{};
	};

} // namespace CE

#include "Definitions.rtti.h"
