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
		
		component->parentComponent = this;
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

	void SceneComponent::Tick(f32 delta)
	{
		Super::Tick(delta);

		for (auto component : attachedComponents)
		{
			component->Tick(delta);
		}
	}

} // namespace CE

