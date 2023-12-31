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
			return srgType;
		}

		inline ShaderResourceGroupLayout GetLayout() const
		{
			return srgLayout;
		}

	protected:

		SRGType srgType{};

		ShaderResourceGroupLayout srgLayout{};

	};
    
} // namespace CE::RHI

