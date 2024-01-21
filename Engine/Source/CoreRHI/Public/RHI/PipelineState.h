#pragma once

namespace CE::RHI
{
	enum class PipelineStateType
	{
		Graphics,
		Compute,
		RayTracing,
		COUNT
	};

	enum class VertexAttributeDataType
	{
		Undefined,
		Float, Float2, Float3, Float4,
		Int, Int2, Int3, Int4
	};
	ENUM_CLASS(VertexAttributeDataType);

	struct VertexAttributeDescriptor
	{
		VertexAttributeDataType dataType = VertexAttributeDataType::Undefined;
		u32 location = 0;
		u32 offset = 0;
		u32 inputSlot = 0;

		inline SIZE_T GetHash() const
		{
			if (dataType == VertexAttributeDataType::Undefined)
				return 0;

			SIZE_T hash = CE::GetHash(dataType);
			CombineHash(hash, location);
			CombineHash(hash, offset);
			CombineHash(hash, inputSlot);
			return hash;
		}
	};

	enum class VertexInputRate
	{
		PerVertex = 0,
		PerInstance
	};

	struct VertexInputSlotDescriptor
	{
		u32 inputSlot = 0;
		u32 stride = 0;
		VertexInputRate inputRate = VertexInputRate::PerVertex;

		inline SIZE_T GetHash() const
		{
			SIZE_T hash = CE::GetHash(inputSlot);
			CombineHash(hash, stride);
			CombineHash(hash, inputRate);
			return hash;
		}
	};

	struct ShaderStageDescriptor
	{
		RHI::ShaderModule* shaderModule = nullptr;
		Name entryPoint = "";

		RHI::ShaderStage GetShaderStage() const;

		SIZE_T GetHash() const;
	};

	struct PipelineDescriptor
	{
		// Used for debugging purposes.
		Name name{};

		Array<ShaderStageDescriptor> shaderStages{};
		Array<ShaderResourceGroupLayout> srgLayouts{};

		SIZE_T GetHash() const;
	};

	enum class BlendOp
	{
		Add,
		Subtract,
		ReverseSubtract,
		Min, Max
	};

	enum class BlendFactor
	{
		Zero,
		One,
		SrcColor,
		OneMinusSrcColor,
		DstColor,
		OneMinusDstColor,
		SrcAlpha,
		OneMinusSrcAlpha,
		DstAlpha,
		OneMinusDstAlpha
	};

	enum class CompareOp
	{
		Never = 0,
		Less = 1,
		Equal = 2,
		LessOrEqual = 3,
		Greater = 4,
		NotEqual = 5,
		GreaterOrEqual = 6,
		Always = 7,
	};

	enum class StencilOp
	{
		Keep,
		Zero,
		Replace,
		IncrementAndClamp,
		DecrementAndClamp,
		Invert,
		Increment,
		Decrement
	};

	struct ColorBlendState
	{
		bool blendEnable = true;
		BlendOp colorBlendOp = BlendOp::Add;
		BlendFactor srcColorBlend = BlendFactor::SrcAlpha;
		BlendFactor dstColorBlend = BlendFactor::OneMinusSrcAlpha;
		BlendOp alphaBlendOp = BlendOp::Add;
		BlendFactor srcAlphaBlend = BlendFactor::One;
		BlendFactor dstAlphaBlend = BlendFactor::Zero;
		ColorComponentMask componentMask = ColorComponentMask::All;

		inline SIZE_T GetHash() const
		{
			SIZE_T hash = CE::GetHash(blendEnable);
			if (!blendEnable)
				return hash;
			CombineHash(hash, colorBlendOp);
			CombineHash(hash, srcColorBlend);
			CombineHash(hash, dstColorBlend);
			CombineHash(hash, alphaBlendOp);
			CombineHash(hash, srcAlphaBlend);
			CombineHash(hash, dstAlphaBlend);
			CombineHash(hash, componentMask);
			return hash;
		}
	};

	struct BlendState
	{
		Array<ColorBlendState> colorBlends{};

		inline SIZE_T GetHash() const
		{
			SIZE_T hash = 0;
			for (int i = 0; i < colorBlends.GetSize(); i++)
			{
				if (i == 0)
					hash = colorBlends[i].GetHash();
				else
					CombineHash(hash, colorBlends[i]);
			}
			return hash;
		}
	};

	struct DepthState
	{
		bool enable = false;
		CompareOp compareOp = CompareOp::Less;
		bool testEnable = false;
		bool writeEnable = false;

		inline SIZE_T GetHash() const
		{
			SIZE_T hash = CE::GetHash(enable);
			if (!enable)
				return hash;
			CombineHash(hash, compareOp);
			CombineHash(hash, testEnable);
			CombineHash(hash, writeEnable);
			return hash;
		}
	};

	struct StencilOpState
	{
		StencilOp failOp = StencilOp::Keep;
		StencilOp depthFailOp = StencilOp::Keep;
		StencilOp passOp = StencilOp::Keep;
		CompareOp compareOp = CompareOp::Always;

		inline SIZE_T GetHash() const
		{
			SIZE_T hash = CE::GetHash(failOp);
			CombineHash(hash, depthFailOp);
			CombineHash(hash, passOp);
			CombineHash(hash, compareOp);
			return hash;
		}
	};

	struct StencilState
	{
		bool enable = false;
		u32 readMask = 0xFF;
		u32 writeMask = 0xFF;
		StencilOpState frontFace{};
		StencilOpState backFace{};

		inline SIZE_T GetHash() const
		{
			SIZE_T hash = CE::GetHash(enable);
			if (!enable)
				return hash;
			CombineHash(hash, readMask);
			CombineHash(hash, writeMask);
			CombineHash(hash, frontFace);
			CombineHash(hash, backFace);
			return hash;
		}
	};

	struct DepthStencilState
	{
		DepthState depthState{};
		StencilState stencilState{};

		inline SIZE_T GetHash() const
		{
			SIZE_T hash = depthState.GetHash();
			CombineHash(hash, stencilState);
			return hash;
		}
	};

	struct RasterState
	{
		CullMode cullMode = CullMode::Back;
		FillMode fillMode = FillMode::Solid;
		bool multisampleEnable = false;
		bool depthClipEnable = false;

		inline SIZE_T GetHash() const
		{
			SIZE_T hash = CE::GetHash(cullMode);
			CombineHash(hash, fillMode);
			CombineHash(hash, multisampleEnable);
			CombineHash(hash, depthClipEnable);
			return hash;
		}
	};

	struct GraphicsPipelineDescriptor : PipelineDescriptor
	{
		Array<VertexAttributeDescriptor> vertexAttributes{};
		Array<VertexInputSlotDescriptor> vertexInputSlots{};
		
		RasterState rasterState{};
		BlendState blendState{};
		DepthStencilState depthStencilState{};
		MultisampleState multisampleState{};

		SIZE_T GetHash() const;
	};

	class IPipelineLayout
	{
	protected:

		virtual ~IPipelineLayout() {}

	public:

		virtual bool IsCompatibleWith(IPipelineLayout* other) = 0;

	};

	class PipelineState : public RHIResource
	{
	public:
		virtual ~PipelineState() = default;

		inline PipelineStateType GetPipelineType() const { return pipelineType; }

		inline bool IsGraphicsPipeline() const { return pipelineType == PipelineStateType::Graphics; }
		inline bool IsComputePipeline() const { return pipelineType == PipelineStateType::Compute; }
		inline bool IsRayTracingPipeline() const { return pipelineType == PipelineStateType::RayTracing; }

		virtual IPipelineLayout* GetPipelineLayout() = 0;

	protected:
		PipelineState() : RHIResource(RHI::ResourceType::PipelineState) {}

		PipelineStateType pipelineType{};

	};
    
} // namespace CE::RHI
