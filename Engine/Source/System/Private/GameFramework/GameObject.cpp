
#include "GameFramework/GameObject.h"

#include "GameFramework/GameComponent.h"

namespace CE
{
	GameObject::GameObject()
	{

	}

	GameObject::GameObject(CE::Name name) : Object(name)
	{

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

	void GameObject::AddComponent(TypeId typeId)
	{
		auto typeInfo = CE::GetTypeInfo(typeId);
		if (typeInfo == nullptr)
			return;
		
	}

} // namespace CE

CE_RTTI_CLASS_IMPL(SYSTEM_API, CE, GameObject)

