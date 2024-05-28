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

		void SetRootComponent(SceneComponent* rootComponent);

		template<typename TSceneComponent> requires TIsBaseClassOf<SceneComponent, TSceneComponent>::Value
		TSceneComponent* SetRootComponent(const String& name = "")
		{
			return (TSceneComponent*)SetRootComponent(TSceneComponent::StaticType(), name);
		}

		void AttachActor(Actor* actor);
		void DetachActor(Actor* actor);

		Actor* GetParentActor() const { return parent; }

		CE::Scene* GetScene() const { return scene; }

		virtual void OnBeginPlay();

		virtual void Tick(f32 delta);

		SceneComponent* GetRootComponent() const { return rootComponent; }

		void SetLocalPosition(const Vec3& localPos)
		{
			if (rootComponent)
				rootComponent->SetLocalPosition(localPos);
		}

		bool IsEnabled() const;

		void SetEnabled(bool enabled);

		bool IsSelfEnabled() const { return isEnabled; }

		u32 GetChildCount() const { return children.GetSize(); }

		Actor* GetChild(u32 index) const { return children[index]; }

	protected:

		void OnFieldEdited(FieldType* field) override;

		virtual void OnEnabled();

		virtual void OnDisabled();

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
		CE::Scene* scene = nullptr;

		FIELD(EditAnywhere, Category = "Actor", DisplayName = "Enabled")
		bool isEnabled = true;

		b8 hasBegunPlaying = false;

		friend class CE::Scene;
		friend class SceneComponent;
	};

} // namespace CE

#include "Actor.rtti.h"
