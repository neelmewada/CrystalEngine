
#include "GameFramework/GameObject.h"
#include "GameFramework/GameComponent.h"
#include "GameFramework/Scene.h"

namespace CE
{
	GameObject::GameObject() : Object("GameObject")
	{

	}

	GameObject::GameObject(CE::Name name) : Object(name)
	{

	}

    GameObject::GameObject(Scene* scene, Name name) : Object(name)
    {
        if (scene != nullptr)
        {
            scene->AddGameObject(this);
        }
    }

	GameObject::~GameObject()
	{
        if (owner != nullptr)
        {
            while (components.GetSize() > 0)
            {
                RemoveComponent(components[0]);
            }
            
            owner->RemoveGameObject(this);
            
            for (auto child : children)
            {
                delete child;
            }
            children.Clear();
        }
	}

    GameComponent* GameObject::AddComponent(GameComponent* component)
	{
        if (component == nullptr)
			return nullptr;

        auto componentType = (ClassType*)component->GetType();

        if (components.Exists(component))
        {
            CE_LOG(Error, All, "GameObject::AddComponent(GameComponent*): Attempted to add duplicate components {} of type {}", component->GetName(), componentType->GetName());
            return nullptr;
        }

        if (components.Exists([component](CE::GameComponent* comp) -> bool
            {
                return comp->GetType()->GetTypeId() == component->GetType()->GetTypeId();
            }))
        {
            const auto& localAttribs = componentType->GetLocalAttributes();
            int idx = localAttribs.IndexOf([](const Attribute& attrib) -> bool
                {
                    return attrib.GetKey() == "AllowMultiple";
                });
            if (idx == -1 || localAttribs[idx].GetValue() != "true") // Cannot add multiple components
            {
                CE_LOG(Error, All, "GameObject::AddComponent(GameComponent*): Cannot add multiple components of type {}", componentType->GetName());
                return nullptr;
            }
        }

		components.Add(component);
        component->owner = this;
        
        if (owner != nullptr)
        {
            owner->AddObject(component);
        }
        
        component->Init();
        component->Activate();
        
        return component;
	}

	void GameObject::RemoveComponent(GameComponent* component)
	{
        if (component->IsActive())
            component->Deactivate();
		components.Remove(component);
        component->OnParentComponentRemoved();
        
        if (owner != nullptr)
        {
            owner->RemoveObject(component);
        }
	}

    GameComponent* GameObject::AddComponent(ClassType* componentType)
    {
        if (componentType == nullptr)
        {
            CE_LOG(Error, All, "GameComponent::AddComponent(ClassType*): The componentType is passed as null");
            return nullptr;
        }

        if (!componentType->IsAssignableTo(TYPEID(GameComponent)))
        {
            CE_LOG(Error, All, "GameComponent::AddComponent(ClassType*): The componentType {} passed to AddComponent() does not derive from GameComponent class", componentType->GetName());
            return nullptr;
        }

        if (!componentType->CanBeInstantiated())
        {
            CE_LOG(Error, All, "GameComponent::AddComponent(ClassType*): The component with name {} cannot be instantiated", componentType->GetName());
            return nullptr;
        }

        auto instance = (GameComponent*)componentType->CreateInstance();
        auto result = AddComponent(instance);
        if (result == nullptr)
        {
            componentType->DestroyInstance(instance);
        }
        return result;
    }

    GameComponent* GameObject::AddComponent(TypeId typeId)
	{
		auto typeInfo = CE::GetTypeInfo(typeId);
		if (typeInfo == nullptr || !typeInfo->IsClass())
			return nullptr;
		
		auto classType = (ClassType*)typeInfo;
        return AddComponent(classType);
	}

    void GameObject::RemoveComponent(ClassType* componentClass)
    {
        for (int i = 0; i < components.GetSize(); i++)
        {
            if (components[i] == nullptr)
                continue;
            if (components[i]->GetType() == componentClass)
            {
                RemoveComponent(components[i]);
                components.RemoveAt(i);
                return;
            }
        }
    }

    void GameObject::RemoveComponent(TypeId componentTypeId)
    {
        for (int i = 0; i < components.GetSize(); i++)
        {
	        if (components[i] == nullptr)
                continue;;
            if (components[i]->GetTypeId() == componentTypeId)
            {
                RemoveComponent(components[i]);
                components.RemoveAt(i);
	            return;
            }
        }
    }

	void GameObject::Tick(f32 deltaTime)
	{
		for (int i = 0; i < components.GetSize(); i++)
		{
			if (components[i] == nullptr)
				continue;

			if (components[i]->IsActive())
				components[i]->Tick(deltaTime);
		}
	}

    GameComponent* GameObject::GetComponent(TypeId componentTypeId) const
    {
        for (int i = 0; i < components.GetSize(); i++)
        {
            if (components[i]->GetType()->GetTypeId() == componentTypeId)
            {
                return components[i];
            }
        }
        return nullptr;
    }

    bool GameObject::HasComponent(TypeId componentTypeId)
    {
        for (int i = 0; i < components.GetSize(); i++)
        {
            if (components[i]->GetType()->GetTypeId() == componentTypeId)
            {
                return true;
            }
        }
        return false;
    }

    void GameObject::AddChild(GameObject* go)
    {
        if (go == nullptr)
            return;
        
        go->parent = this;
        go->owner = owner;
        children.Add(go);
        
        if (owner != nullptr)
        {
            owner->AddObject(go);
        }
    }

    void GameObject::RemoveChild(GameObject* go)
    {
        if (go == nullptr)
            return;
        
        go->parent = nullptr;
        children.Remove(go);
        
        if (owner != nullptr)
        {
            owner->RemoveObject(go);
        }
    }

    s32 GameObject::GetIndexInParent()
    {
        if (parent == nullptr)
            return 0;
        
        return parent->GetChildIndex(this);
    }

    s32 GameObject::GetChildIndex(GameObject* child)
    {
        int i = 0;
        for (auto childGO : children)
        {
            if (childGO == child)
                return i;
            i++;
        }
        return -1;
    }

    void GameObject::OnSubComponentAdded(GameComponent* subComponent)
    {
        if (owner != nullptr)
        {
            owner->AddObject(subComponent);
        }
    }

    void GameObject::OnSubComponentRemoved(GameComponent* subComponent)
    {
        if (owner != nullptr)
        {
            owner->RemoveObject(subComponent);
        }
    }

} // namespace CE


