#include "CoreRPI.h"

namespace CE::RPI
{

	Pass::~Pass()
	{
		
	}

	PassAttachmentBinding* Pass::FindInputBinding(const Name& name)
	{
		for (int i = 0; i < inputBindings.GetSize(); i++)
		{
			if (inputBindings[i].name == name)
				return &inputBindings[i];
		}
		return nullptr;
	}

	PassAttachmentBinding* Pass::FindInputOutputBinding(const Name& name)
	{
		for (int i = 0; i < inputOutputBindings.GetSize(); i++)
		{
			if (inputOutputBindings[i].name == name)
				return &inputOutputBindings[i];
		}
		return nullptr;
	}

	PassAttachmentBinding* Pass::FindOutputBinding(const Name& name)
	{
		for (int i = 0; i < outputBindings.GetSize(); i++)
		{
			if (outputBindings[i].name == name)
				return &outputBindings[i];
		}
		return nullptr;
	}

	PassAttachmentBinding* Pass::FindBinding(const Name& name)
	{
		auto result = FindInputOutputBinding(name);
		if (result) return result;
		result = FindInputBinding(name);
		if (result) return result;
		result = FindOutputBinding(name);
		if (result) return result;
		return nullptr;
	}

    Pass::Pass()
    {
		if (drawListTag.IsValid())
		{
			RPISystem::Get().GetDrawListTagRegistry()->ReleaseTag(drawListTag);
		}

    }

} // namespace CE::RPI