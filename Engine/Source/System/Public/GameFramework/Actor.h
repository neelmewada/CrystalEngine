#pragma once

namespace CE
{
	class ActorComponent;
	class SceneComponent;

	CLASS()
	class SYSTEM_API Actor : public Object
	{
		CE_CLASS(Actor, Object)
	public:

		Actor();

		// - Public API -

	protected:


	private:

		FIELD()
		SceneComponent* rootComponent = nullptr;

		FIELD()
		Array<ActorComponent*> attachedComponents{};
	};

} // namespace CE

#include "Actor.rtti.h"
