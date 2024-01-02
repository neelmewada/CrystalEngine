#pragma once

namespace CE::RHI
{
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

	class IPipelineLayout
	{
	public:
		virtual ~IPipelineLayout() {}

		virtual PipelineType GetPipelineType() = 0;

	};

	class IPipelineState
	{
	public:

		virtual ~IPipelineState() {}

		virtual bool IsGraphicsPipelineState() = 0;
		virtual bool IsComputePipelineState() = 0;

		virtual void* GetNativeHandle() = 0;

		virtual IPipelineLayout* GetPipelineLayout() = 0;

	};

	class CORERHI_API GraphicsPipelineState : public RHIResource, public IPipelineState
	{
	protected:
		GraphicsPipelineState() : RHIResource(ResourceType::GraphicsPipelineState)
		{}

	public:
		virtual ~GraphicsPipelineState() = default;

		// - Public API -

	};
    
} // namespace CE::RHI
