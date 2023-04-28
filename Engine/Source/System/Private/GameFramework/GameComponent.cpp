
#include "GameFramework/GameComponent.h"

namespace CE
{
    GameComponent::GameComponent()
	    : Component("GameComponent")
    {

    }

    GameComponent::GameComponent(CE::Name name)
        : Component(name)
    {
        
    }

    GameComponent::~GameComponent()
    {
        if (owner != nullptr)
        {
            owner->RemoveComponent(this);
        }

        for (auto subComponent : subComponents)
        {
            if (subComponent->IsActive())
                subComponent->Deactivate();
	        if (owner != nullptr)
	        {
                owner->OnSubComponentRemoved(subComponent);
	        }

            delete subComponent;
        }
        subComponents.Clear();
    }

    void GameComponent::Activate()
    {
        Super::Activate();
    }

    void GameComponent::Deactivate()
    {
        Super::Deactivate();
    }

    GameComponent* GameComponent::AddSubComponent(ClassType* componentClass)
    {
        if (componentClass == nullptr || !componentClass->IsAssignableTo(TYPEID(GameComponent)) ||
            !componentClass->CanBeInstantiated())
            return nullptr;
        GameComponent* subComponent = (GameComponent*)componentClass->CreateInstance();
        if (subComponent == nullptr)
            nullptr;
        return AddSubComponent(subComponent);
    }

    GameComponent* GameComponent::AddSubComponent(TypeId typeId)
    {
        ClassType* componentClass = ClassType::FindClassById(typeId);
        return AddSubComponent(componentClass);
    }

    void GameComponent::RemoveSubComponent(ClassType* componentClass)
    {
        for (int i = 0; i < subComponents.GetSize(); i++)
        {
            if (subComponents[i] == nullptr)
                continue;;
            if (subComponents[i]->GetType() == componentClass)
            {
                RemoveSubComponent(subComponents[i]);
                subComponents.RemoveAt(i);
                return;
            }
        }
    }

    void GameComponent::RemoveSubComponent(TypeId componentTypeId)
    {
        for (int i = 0; i < subComponents.GetSize(); i++)
        {
            if (subComponents[i] == nullptr)
                continue;;
            if (subComponents[i]->GetTypeId() == componentTypeId)
            {
                RemoveSubComponent(subComponents[i]);
                subComponents.RemoveAt(i);
                return;
            }
        }
    }

    GameComponent* GameComponent::AddSubComponent(GameComponent* subComponent)
    {
        if (subComponent == nullptr)
            return nullptr;
        if (owner != nullptr)
        {
            subComponent->owner = owner;
	        owner->OnSubComponentAdded(subComponent);
        }
        subComponent->parent = this;
        subComponents.Add(subComponent);
        return subComponent;
    }

    void GameComponent::RemoveSubComponent(GameComponent* subComponent)
    {
        if (subComponent == nullptr)
            return;
        subComponents.Remove(subComponent);
        subComponent->parent = nullptr;
        if (owner != nullptr)
        {
            owner->OnSubComponentRemoved(subComponent);
            subComponent->owner = nullptr;
        }
    }

    void GameComponent::OnParentComponentRemoved()
    {
        while (subComponents.GetSize() > 0)
        {
            RemoveSubComponent(subComponents[0]);
        }
    }

} // namespace CE

