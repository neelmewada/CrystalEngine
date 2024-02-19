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

		inline Actor* GetActor() const { return owner; }

		CE::Scene* GetScene() const;

		inline bool IsEnabled() const { return isEnabled; }

		inline void SetEnabled(bool enabled) { isEnabled = enabled; }

		inline bool CanTick() const { return canTick; }

		inline void SetCanTick(bool canTick) { this->canTick = canTick; }

		inline bool HasBegunPlaying() const { return hasBegunPlaying; }

	protected:

		FIELD()
		bool canTick = true;

		FIELD()
		bool isEnabled = true;

		FIELD()
		Actor* owner = nullptr;

	private:

		b8 hasBegunPlaying = false;

		friend class Actor;
	};

} // namespace CE

#include "ActorComponent.rtti.h"
