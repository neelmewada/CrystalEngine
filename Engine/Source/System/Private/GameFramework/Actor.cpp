#include "System.h"

namespace CE
{

	Actor::Actor()
	{
		rootComponent = nullptr;
	}

	SceneComponent* Actor::SetRootComponent(SubClassType<SceneComponent> componentType, String name)
	{
		if (componentType == nullptr)
			return nullptr;
		if (rootComponent)
			rootComponent->Destroy();
		rootComponent = nullptr;
		
		if (name.IsEmpty())
			name = componentType->GetName().GetLastComponent();
		rootComponent = CreateObject<SceneComponent>(this, name, OF_NoFlags, componentType);

		if (scene)
		{
			scene->OnRootComponentSet(rootComponent, this);
		}

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

			if (scene)
			{
				scene->OnRootComponentSet(rootComponent, this);
			}
		}
	}

	void Actor::AttachActor(Actor* actor)
	{
		if (!actor)
			return;
		
		children.Add(actor);
		actor->parent = this;
		actor->scene = scene;

		if (actor->parent != nullptr && actor->parent != this)
		{
			actor->parent->DetachActor(actor);
		}

		if (actor->parent != this)
		{
			AttachSubobject(actor);
		}
        
        if (scene != nullptr)
        {
            scene->OnActorChainAttached(actor);
        }
        
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
        
        if (scene != nullptr)
        {
            scene->OnActorChainDetached(actor);
        }
        
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

	void Actor::OnBeginPlay()
	{
		if (rootComponent)
			rootComponent->OnBeginPlay();

		for (auto component : attachedComponents)
		{
			component->OnBeginPlay();
		}

		for (auto child : children)
		{
			child->OnBeginPlay();
		}

		hasBegunPlaying = true;
	}

	void Actor::Tick(f32 delta)
    {
		if (!IsSelfEnabled())
			return;

		if (rootComponent && rootComponent->CanTick() && rootComponent->IsSelfEnabled())
		{
			rootComponent->Tick(delta);
		}

		for (auto component : attachedComponents)
		{
			if (component->CanTick() && component->IsSelfEnabled())
			{
				component->Tick(delta);
			}
		}

		for (auto child : children)
		{
			if (child->IsSelfEnabled())
			{
				child->Tick(delta);
			}
		}
    }

	bool Actor::IsEnabled() const
	{
		if (!parent)
			return isEnabled;

		return isEnabled && parent->IsEnabled();
	}

	void Actor::SetEnabled(bool enabled)
	{
		if (isEnabled == enabled)
			return;

		isEnabled = enabled;

		if (isEnabled)
		{
			OnEnabled();

			if (rootComponent != nullptr)
			{
				rootComponent->OnEnabled();
			}

			for (ActorComponent* attachedComponent : attachedComponents)
			{
				attachedComponent->OnEnabled();
			}

			for (Actor* child : children)
			{
				child->OnEnabled();
			}
		}
		else
		{
			OnDisabled();

			if (rootComponent != nullptr)
			{
				rootComponent->OnDisabled();
			}

			for (ActorComponent* attachedComponent : attachedComponents)
			{
				attachedComponent->OnDisabled();
			}

			for (Actor* child : children)
			{
				child->OnDisabled();
			}
		}
	}

	void Actor::OnFieldEdited(FieldType* field)
	{
		Super::OnFieldEdited(field);

		thread_local const Name isEnabledName = "isEnabled";

		if (field->GetName() == isEnabledName)
		{
			if (IsEnabled())
				OnEnabled();
			else
				OnDisabled();
		}
	}

	void Actor::OnEnabled()
	{
		if (rootComponent != nullptr && rootComponent->IsEnabled())
		{
			rootComponent->OnEnabled();
		}

		for (ActorComponent* attachedComponent : attachedComponents)
		{
			if (attachedComponent->IsEnabled())
				attachedComponent->OnEnabled();
		}

		for (Actor* child : children)
		{
			if (child->IsEnabled())
				child->OnEnabled();
		}
	}

	void Actor::OnDisabled()
	{
		if (rootComponent != nullptr && !rootComponent->IsEnabled())
		{
			rootComponent->OnDisabled();
		}

		for (ActorComponent* attachedComponent : attachedComponents)
		{
			if (!attachedComponent->IsEnabled())
				attachedComponent->OnDisabled();
		}

		for (Actor* child : children)
		{
			if (!child->IsEnabled())
				child->OnDisabled();
		}
	}

} // namespace CE
