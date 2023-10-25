#include "System.h"

namespace CE
{

	Actor::Actor()
	{
		rootComponent = nullptr;//CreateDefaultSubobject<SceneComponent>(TEXT("RootComponent"));
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

	void Actor::SetRootComponent(SceneComponent* component)
	{
		if (rootComponent)
		{
			// If current rootComponent's lifecycle is managed by `this` actor, destroy it.
			// Otherwise, just detach the subobject without destroying it.
			if (IsObjectPresentInHierarchy(rootComponent))
			{
				rootComponent->Destroy();
			}
			else
			{
				DetachSubobject(rootComponent);
			}

			rootComponent = nullptr;
		}

		rootComponent = component;

		if (rootComponent) // component can be set to nullptr
		{
			
		}
	}

	void Actor::AttachActor(Actor* actor)
	{
		if (!actor)
			return;
		
		children.Add(actor);
		actor->parent = this;
		actor->scene = scene;

		AttachSubobject(actor);
	}

	void Actor::DetachActor(Actor* actor)
	{
		if (!actor)
			return;

		children.Remove(actor);
		actor->parent = nullptr;
		actor->scene = nullptr;

		DetachSubobject(actor);
	}

} // namespace CE
