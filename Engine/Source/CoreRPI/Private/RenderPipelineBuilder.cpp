#include "CoreRPI.h"

namespace CE::RPI
{

    RenderPipeline* RenderPipelineBuilder::Build()
    {
		
        return nullptr;
    }

	PassDefinition* RenderPipelineDescriptor::FindPassDefinition(const Name& passDefinition)
	{
		for (int i = 0; i < passDefinitions.GetSize(); i++)
		{
			if (passDefinitions[i].name == passDefinition)
				return &passDefinitions[i];
		}

		return nullptr;
	}

} // namespace CE::RPI
