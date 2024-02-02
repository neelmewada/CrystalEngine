#pragma once

namespace CE::RPI
{

	class CORERPI_API ShaderResourceGroup
	{
	public:
		ShaderResourceGroup(RHI::ShaderResourceGroup* srg);

		~ShaderResourceGroup();

	private:

		RHI::ShaderResourceGroup* rhiShaderResourceGroup = nullptr;

	};
    
} // namespace CE::RPI
