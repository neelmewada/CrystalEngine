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

		inline SRGType GetSRGType() const
		{
			return srgLayout.srgType;
		}

		inline ShaderResourceGroupLayout GetLayout() const
		{
			return srgLayout;
		}

	protected:

		ShaderResourceGroupLayout srgLayout{};

	};
    
} // namespace CE::RHI

