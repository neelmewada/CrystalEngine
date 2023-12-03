#include "CoreRPI.h"

namespace CE::RPI
{
    
	ShaderResourceGroup::ShaderResourceGroup(RHI::ShaderResourceGroup* srg)
		: rhiShaderResourceGroup(srg)
	{
		
	}

	ShaderResourceGroup::ShaderResourceGroup(const RHI::ShaderResourceGroupDesc& desc)
	{
		rhiShaderResourceGroup = RHI::gDynamicRHI->CreateShaderResourceGroup(desc);
	}

	ShaderResourceGroup::~ShaderResourceGroup()
	{
		if (rhiShaderResourceGroup != nullptr)
		{
			RHI::gDynamicRHI->DestroyShaderResourceGroup(rhiShaderResourceGroup);
			rhiShaderResourceGroup = nullptr;
		}
	}

} // namespace CE::RPI
