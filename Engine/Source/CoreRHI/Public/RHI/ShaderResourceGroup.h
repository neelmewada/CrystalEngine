#pragma once

namespace CE::RHI
{

	class CORERHI_API ShaderResourceGroup : public RHIResource
	{
	protected:
		ShaderResourceGroup() : RHIResource(ResourceType::ShaderResourceGroup)
		{}

	public:

		virtual bool Bind(Name name, RHI::Buffer* buffer, SIZE_T offset = 0, SIZE_T size = 0) = 0;

		virtual int GetFrequencyId() const = 0;

		virtual SRGType GetSRGType() const = 0;

	protected:

	};
    
} // namespace CE::RHI

