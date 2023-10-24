#pragma once

namespace CE
{
	class Scene;
	class ActorComponent;
	class SceneComponent;

	CLASS()
	class SYSTEM_API Actor : public Object
	{
		CE_CLASS(Actor, Object)
	public:

		Actor();

		// - Public API -

		SceneComponent* SetRootComponent(SubClassType<SceneComponent> componentType, String name = "");

		template<typename TSceneComponent> requires TIsBaseClassOf<SceneComponent, TSceneComponent>::Value
		FORCE_INLINE TSceneComponent* SetRootComponent(const String& name = "")
		{
			return (TSceneComponent*)SetRootComponent(TSceneComponent::StaticType(), name);
		}

		void AttachActor(Actor* actor);
		void DetachActor(Actor* actor);

		inline Actor* GetParent() const { return parent; }

		inline Scene* GetScene() const { return scene; }

	protected:


	private:

		FIELD()
		SceneComponent* rootComponent = nullptr;

		FIELD()
		Array<ActorComponent*> attachedComponents{};

		FIELD()
		Array<Actor*> children{};

		FIELD()
		Actor* parent = nullptr;

		FIELD()
		Scene* scene = nullptr;

		friend class Scene;
	};

} // namespace CE

#include "Actor.rtti.h"
