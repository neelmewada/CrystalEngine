#pragma once

namespace CE
{
	class Actor;
	class SceneComponent;

	CLASS()
	class SYSTEM_API ActorComponent : public Object
	{
		CE_CLASS(ActorComponent, Object)
	public:

		ActorComponent();

		// Public API

		virtual bool IsSceneComponent() const { return false; }

		virtual void Tick(f32 delta);

		inline Actor* GetActor() const { return owner; }

	protected:

		FIELD()
		Actor* owner = nullptr;

		friend class Actor;
	};

} // namespace CE

#include "ActorComponent.rtti.h"
