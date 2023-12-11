#include "CoreRPI.h"

namespace CE::RPI
{

    ShaderVariant::ShaderVariant(const ShaderReflection& reflectionInfo)
		: reflectionInfo(reflectionInfo)
    {

    }

	ShaderVariant::~ShaderVariant()
	{
		for (auto [stage, module] : modulesByStage)
		{
			if (module == nullptr)
				continue;
			RHI::gDynamicRHI->DestroyShaderModule(module);
		}
		modulesByStage.Clear();
	}

} // namespace CE::RPI
