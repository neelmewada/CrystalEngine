#pragma once

namespace CE::RHI
{
	enum class PipelineStateType
	{
		Graphics = 0,
		Compute,
		RayTracing,
		COUNT
	};

	enum class VertexAttributeDataType
	{
		Undefined = 0,
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

	struct GraphicsPipelineDescriptor
	{
		u32 vertexStrideInBytes = 0;
		Array<VertexAttributeDescriptor> vertexAttribs{};
		Array<ShaderStageDescriptor> shaderStages{};
		Array<ShaderResourceGroupLayout> srgLayouts{};

		CullMode cullMode = CullMode::Back;
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
