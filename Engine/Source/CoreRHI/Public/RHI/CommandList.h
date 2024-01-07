#pragma once

namespace CE::RHI
{
	class IPipelineState;
	class Viewport;
	class ShaderResourceGroup;

	class CORERHI_API CommandList : public RHIResource
	{
	protected:
		CommandList() : RHIResource(ResourceType::CommandList)
		{}
	public:
		virtual ~CommandList() = default;

		// - Public API -

		virtual CommandListType GetCommandListType() = 0;

		// - Command List API -

		virtual void Begin() = 0;
		virtual void End() = 0;

	};

	class CORERHI_API GraphicsCommandList : public CommandList
	{
	protected:
		GraphicsCommandList() : CommandList()
		{}

	public:
		virtual ~GraphicsCommandList() = default;

		// - Public API -

		virtual CommandListType GetCommandListType() override final { return CommandListType::Graphics; }

		// - Graphics Command List API -


	};
    
} // namespace CE::RHI
