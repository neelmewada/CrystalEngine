#pragma once

namespace CE
{
	class Actor;

	CLASS()
	class SYSTEM_API Scene : public Asset
	{
		CE_CLASS(Scene, Asset)
	public:

		Scene();
		virtual ~Scene();

		inline Actor* GetRootActor() const
		{
			return root;
		}
        
		virtual void Tick(f32 delta);

	system_internal:

		// - Internal API -

		void OnActorChainAttached(Actor* actor);
		void OnActorChainDetached(Actor* actor);

	protected:

		FIELD()
		Actor* root = nullptr;

		// - Cache -

		HashMap<UUID, Actor*> actorInstancesByUuid{};

		friend class Actor;
		friend class ActorComponent;
		friend class SceneComponent;
	};
    
} // namespace CE

#include "Scene.rtti.h"
