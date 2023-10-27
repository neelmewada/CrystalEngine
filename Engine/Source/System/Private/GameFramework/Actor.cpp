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
			rootComponent->owner = this;
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
        
        std::function<void(Actor*)> recursivelySetScene = [&](Actor* inActor)
        {
            inActor->scene = scene;
            
            for (auto child : inActor->children)
            {
                recursivelySetScene(child);
            }
        };
        
        recursivelySetScene(actor);
	}

	void Actor::DetachActor(Actor* actor)
	{
		if (!actor)
			return;
		s64 index = children.IndexOf(actor);
		if (index < 0)
			return;
        
		children.RemoveAt(index);
		actor->parent = nullptr;
		actor->scene = nullptr;
        
		DetachSubobject(actor);
        
        std::function<void(Actor*)> recursivelyResetScene = [&](Actor* inActor)
        {
            inActor->scene = nullptr;
            
            for (auto child : inActor->children)
            {
                recursivelyResetScene(child);
            }
        };
        
        recursivelyResetScene(actor);
	}

    void Actor::Tick(f32 delta)
    {
		if (rootComponent)
			rootComponent->Tick(delta);

		for (auto component : attachedComponents)
		{
			component->Tick(delta);
		}

		for (auto child : children)
		{
			child->Tick(delta);
		}
    }

} // namespace CE
