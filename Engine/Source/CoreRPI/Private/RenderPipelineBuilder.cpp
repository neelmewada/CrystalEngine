#include "CoreRPI.h"

namespace CE::RPI
{
	PassDefinition* RenderPipelineDescriptor::FindPassDefinition(const Name& passDefinition)
	{
		for (int i = 0; i < passDefinitions.GetSize(); i++)
		{
			if (passDefinitions[i].name == passDefinition)
				return &passDefinitions[i];
		}

		return nullptr;
	}

	PassRequest* RenderPipelineDescriptor::FindPassRequest(const Name& passName, PassRequest* parent)
	{
		if (parent == nullptr)
			parent = &rootPass;

		for (int i = 0; i < parent->childPasses.GetSize(); i++)
		{
			if (parent->childPasses[i].passName == passName)
				return &parent->childPasses[i];
			PassRequest* result = FindPassRequest(passName, &parent->childPasses[i]);
			if (result)
				return result;
		}
		return nullptr;
	}

	PassDefinition* RenderPipelineDescriptor::FindPassDefinitionForPassRequest(const Name& passName)
	{
		PassRequest* passRequest = FindPassRequest(passName);
		if (!passRequest)
			return nullptr;

		return FindPassDefinition(passRequest->passDefinition);
	}

    RenderPipeline* RenderPipelineBuilder::Build()
    {
		
        return nullptr;
    }

} // namespace CE::RPI
