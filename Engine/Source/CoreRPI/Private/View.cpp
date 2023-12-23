#include "CoreRPI.h"

namespace CE::RPI
{
	View::View()
	{
		
	}

	View::~View()
	{
		if (shaderResourceGroup != nullptr)
		{
			delete shaderResourceGroup;
			shaderResourceGroup = nullptr;
		}
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
