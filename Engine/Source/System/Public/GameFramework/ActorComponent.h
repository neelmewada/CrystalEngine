#pragma once

namespace CE
{
	class Actor;
	class Scene;
	class SceneComponent;

	CLASS()
	class SYSTEM_API ActorComponent : public Object
	{
		CE_CLASS(ActorComponent, Object)
	public:

		ActorComponent();

		// Public API

		virtual bool IsSceneComponent() const { return false; }

		virtual void OnBeginPlay();

		virtual void Tick(f32 delta);

		Actor* GetActor() const { return owner; }

		CE::Scene* GetScene() const;

		virtual bool IsEnabled() const;

		bool IsSelfEnabled() const { return isEnabled; }

		void SetEnabled(bool enabled);

		bool CanTick() const { return canTick; }

		void SetCanTick(bool canTick) { this->canTick = canTick; }

		bool HasBegunPlaying() const { return hasBegunPlaying; }

	protected:

		void OnFieldValidate(FieldType* field) override;

		virtual void OnEnabled();

		virtual void OnDisabled();

		FIELD()
		bool canTick = true;

		FIELD()
		Actor* owner = nullptr;

	private:

		FIELD(EditAnywhere, Category = "Component")
		bool isEnabled = true;

		b8 hasBegunPlaying = false;
		b8 onEnabledCalled = false;
		b8 onDisabledCalled = false;

		friend class Actor;
		friend class RendererSubsystem;
	};

} // namespace CE

#include "ActorComponent.rtti.h"
