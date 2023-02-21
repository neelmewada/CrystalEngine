
#include "GameFramework/GameObject.h"
#include "GameFramework/GameComponent.h"
#include "GameFramework/Scene.h"

namespace CE
{
	GameObject::GameObject()
	{

	}

	GameObject::GameObject(CE::Name name) : Object(name)
	{

	}

    GameObject::GameObject(Scene* scene) : Object()
    {
        if (scene != nullptr)
        {
            scene->AddGameObject(this);
        }
    }

	GameObject::~GameObject()
	{
        
	}

    GameComponent* GameObject::AddComponent(GameComponent* component)
	{
		if (component == nullptr || components.Exists(component))
			return nullptr;

		components.Add(component);
        component->owner = this;
        
        if (owner != nullptr)
        {
            owner->objects.AddObject(component);
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
        
        if (owner != nullptr)
        {
            owner->objects.RemoveObject(component);
        }
	}

	GameComponent* GameObject::AddComponent(TypeId typeId)
	{
		auto typeInfo = CE::GetTypeInfo(typeId);
		if (typeInfo == nullptr || !typeInfo->IsClass())
			return nullptr;
		
		auto classType = (ClassType*)typeInfo;
		if (!classType->IsAssignableTo(TYPEID(GameComponent)))
        {
            CE_LOG(Error, All, "GameComponent::AddComponent(): The TypeId passed {} to AddComponent() doesn't derive from GameComponent class", typeId);
            return nullptr;
        }
        
        if (!classType->CanBeInstantiated())
        {
            CE_LOG(Error, All, "GameComponent::AddComponent(): The component with TypeId {} cannot be instantiated", typeId);
            return nullptr;
        }

		auto instance = (GameComponent*)classType->CreateDefaultInstance();
		return AddComponent(instance);
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

} // namespace CE

CE_RTTI_CLASS_IMPL(SYSTEM_API, CE, GameObject)

