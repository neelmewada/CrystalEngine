#include "CoreRPI.h"

namespace CE::RPI
{
	View::View()
	{
		RHI::ShaderResourceGroupDesc desc{};
		desc.srgType = RHI::SRGType::PerView;
		
		shaderResourceGroup = new RPI::ShaderResourceGroup(desc);
	}

	View::~View()
	{
		delete shaderResourceGroup;
		shaderResourceGroup = nullptr;
	}

} // namespace CE::RPI
