#include "System.h"

namespace CE
{

	Actor::Actor()
	{
		rootComponent = CreateDefaultSubobject<SceneComponent>(TEXT("RootComponent"));
	}

	SceneComponent* Actor::SetRootComponent(SubClassType<SceneComponent> componentType, String name)
	{
		if (componentType == nullptr)
			return nullptr;
		if (rootComponent)
			rootComponent->Destroy();
		
		if (name.IsEmpty())
			name = componentType->GetName().GetLastComponent();
		rootComponent = CreateObject<SceneComponent>(this, name, OF_NoFlags, componentType);

		return rootComponent;
	}

	void Actor::AttachActor(Actor* actor)
	{
		if (!actor)
			return;

		children.Add(actor);
		actor->parent = this;
	}

} // namespace CE
