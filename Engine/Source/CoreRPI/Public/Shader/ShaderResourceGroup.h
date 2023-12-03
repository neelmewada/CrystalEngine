#pragma once

namespace CE::RPI
{

	class CORERPI_API ShaderResourceGroup
	{
	public:
		ShaderResourceGroup(RHI::ShaderResourceGroup* srg);
		ShaderResourceGroup(const RHI::ShaderResourceGroupDesc& desc);

		~ShaderResourceGroup();

	private:

		RHI::ShaderResourceGroup* rhiShaderResourceGroup = nullptr;

	};
    
} // namespace CE::RPI
