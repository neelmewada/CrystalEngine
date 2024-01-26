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

		inline const ShaderResourceGroupLayout& GetLayout() const
		{
			return srgLayout;
		}

		inline bool IsCompiled() const
		{
			return isCompiled;
		}

		/// @brief Compile the Shader Resource Group. You cannot bind new buffers or images to the variables
		/// in this SRG once it's compiled. You will have to create a new SRG to do so.
		virtual void Compile() = 0;

	protected:

		b8 isCompiled = false;

		b8 isCommitted = false;
		SRGType srgType{};

		ShaderResourceGroupLayout srgLayout{};

	};
    
} // namespace CE::RHI

