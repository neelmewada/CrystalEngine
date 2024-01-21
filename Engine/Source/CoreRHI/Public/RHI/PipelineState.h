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
	};

	struct ShaderStageDescriptor
	{
		RHI::ShaderModule* shaderModule = nullptr;
		RHI::ShaderStage stage = RHI::ShaderStage::None;
		Name entryPoint = "";
	};

	struct PipelineDescriptor
	{
		Array<ShaderStageDescriptor> shaderStages{};
		Array<ShaderResourceGroupLayout> srgLayouts{};
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

	struct ColorBlendState
	{
		bool blendEnable = true;
		BlendOp colorBlendOp = BlendOp::Add;
		BlendFactor srcColorBlend = BlendFactor::SrcAlpha;
		BlendFactor dstColorBlend = BlendFactor::OneMinusSrcAlpha;
		BlendOp alphaBlendOp = BlendOp::Add;
		BlendFactor srcAlphaBlend = BlendFactor::One;
		BlendFactor dstAlphaBlend = BlendFactor::Zero;
	};

	struct DepthStencilState
	{
		CompareOp compareOp = CompareOp::Less;
		bool depthTestEnable = false;
		bool depthWriteEnable = false;
		bool stencilEnable = false;
	};

	struct GraphicsPipelineDescriptor : PipelineDescriptor
	{
		u32 vertexStrideInBytes = 0;
		Array<VertexAttributeDescriptor> vertexAttribs{};
		CullMode cullMode = CullMode::Back;
		List<ColorBlendState> colorBlends{};
		DepthStencilState depthStencil{};
	};


	class PipelineState : public RHIResource
	{
	public:
		virtual ~PipelineState() = default;

		inline PipelineStateType GetPipelineType() const { return pipelineType; }

		inline bool IsGraphicsPipeline() const { return pipelineType == PipelineStateType::Graphics; }
		inline bool IsComputePipeline() const { return pipelineType == PipelineStateType::Compute; }
		inline bool IsRayTracingPipeline() const { return pipelineType == PipelineStateType::RayTracing; }

	protected:
		PipelineState() : RHIResource(RHI::ResourceType::PipelineState) {}

		PipelineStateType pipelineType{};

	};
    
} // namespace CE::RHI
