#pragma once

namespace CE::RHI
{
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

		virtual void SetShaderResourceGroups(const ArrayView<RHI::ShaderResourceGroup*>& srgs) = 0;

	};

} // namespace CE::RHI
