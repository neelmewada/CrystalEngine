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


		// - Command List API -

		virtual void Begin() = 0;
		virtual void End() = 0;

	};

} // namespace CE::RHI
