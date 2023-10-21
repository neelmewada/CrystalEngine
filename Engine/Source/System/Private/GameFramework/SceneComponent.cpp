#include "System.h"

namespace CE
{

    SceneComponent::SceneComponent()
    {
		
    }

	void SceneComponent::SetupAttachment(SceneComponent* component)
	{
		if (!component || component == this)
			return;
		if (ComponentExists(component))
		{
			CE_LOG(Error, All, "SceneComponent::SetupAttachment called with a scene component that already exists in it's hierarcy");
			return;
		}
		
		attachedComponents.Add(component);
	}

	bool SceneComponent::ComponentExists(SceneComponent* component)
	{
		for (auto comp : attachedComponents)
		{
			if (comp == component || comp->ComponentExists(component))
				return true;
		}
		return false;
	}

} // namespace CE

