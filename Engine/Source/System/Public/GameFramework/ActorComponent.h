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



	protected:

		FIELD()
		Actor* owner = nullptr;
	};

} // namespace CE

#include "ActorComponent.rtti.h"
