#include "CoreRPI.h"

namespace CE::RPI
{

	RenderPipeline::RenderPipeline()
	{
		passTree = CreateObject<PassTree>(GetTransientPackage(MODULE_NAME), "PassTree", OF_Transient);
	}

    RenderPipeline::~RenderPipeline()
    {
		if (passTree != nullptr)
		{
			passTree->Destroy();
			passTree = nullptr;
		}
    }

} // namespace CE::RPI
