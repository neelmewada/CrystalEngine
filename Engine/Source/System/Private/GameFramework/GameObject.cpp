
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

	void GameObject::AddComponent(GameComponent* component)
	{
		if (component == nullptr)
			return;

		components.Add(component);
	}

	void GameObject::RemoveComponent(GameComponent* component)
	{
		components.Remove(component);
	}

	GameComponent* GameObject::AddComponent(TypeId typeId)
	{
		auto typeInfo = CE::GetTypeInfo(typeId);
		if (typeInfo == nullptr || !typeInfo->IsClass())
			return nullptr;
		
		auto classType = (ClassType*)typeInfo;
		if (!classType->IsAssignableTo(TYPEID(GameComponent)))
			return nullptr;

		auto instance = (GameComponent*)classType->CreateDefaultInstance();
		AddComponent(instance);

		return instance;
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

} // namespace CE

CE_RTTI_CLASS_IMPL(SYSTEM_API, CE, GameObject)

