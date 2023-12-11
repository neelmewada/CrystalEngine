#include "CoreRPI.h"

namespace CE::RPI
{
	Material::Material()
	{

	}

	Material::~Material()
	{
		if (pipelineState != nullptr)
			RHI::gDynamicRHI->DestroyPipelineState(pipelineState);
		pipelineState = nullptr;
	}
    
} // namespace CE::RPI
