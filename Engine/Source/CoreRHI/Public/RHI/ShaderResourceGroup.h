#pragma once

namespace CE::RHI
{

	class CORERHI_API ShaderResourceGroup : public Resource
	{
	protected:
		ShaderResourceGroup() : Resource(ResourceType::ShaderResourceGroup)
		{}

	public:

		virtual bool Bind(Name name, RHI::Buffer* buffer, SIZE_T offset = 0, SIZE_T size = 0) = 0;

		virtual int GetFrequencyId() const = 0;

		virtual Name GetSRGName() const = 0;

		virtual SRGType GetSRGType() const = 0;

		virtual const RHI::ShaderResourceGroupDesc& GetDesc() const = 0;

	};
    
} // namespace CE::RHI
