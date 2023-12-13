#pragma once

namespace CE::RHI
{

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

	class CORERHI_API GraphicsPipelineState : public Resource, public IPipelineState
	{
	protected:
		GraphicsPipelineState() : Resource(ResourceType::GraphicsPipelineState)
		{}

	public:
		virtual ~GraphicsPipelineState() = default;

		// - Public API -

	};
    
} // namespace CE::RHI
