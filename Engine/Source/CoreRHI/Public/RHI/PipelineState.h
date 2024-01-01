#pragma once

namespace CE::RHI
{

	struct GraphicsPipelineDescriptor
	{
		u32 vertexSizeInBytes = 0;
		Array<VertexAttribDesc> vertexAttribs{};
		Array<ShaderStageDesc> otherStages{};

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
