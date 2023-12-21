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

	void View::SetDrawListMask(const RHI::DrawListMask& mask)
	{
		if (drawListMask != mask)
		{
			drawListMask = mask;
			drawListContext.Shutdown();
			drawListContext.Init(drawListMask);
		}
	}

} // namespace CE::RPI
