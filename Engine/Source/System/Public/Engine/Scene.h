#pragma once

namespace CE
{
	class Actor;
	class ActorComponent;
    class CameraComponent;
    class StaticMeshComponent;

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
        
		virtual void OnBeginPlay();

		virtual void Tick(f32 delta);

		inline CameraComponent* GetMainCamera() const { return mainCamera; }

	system_internal:

		// - Internal API -

		void OnActorChainAttached(Actor* actor);
		void OnActorChainDetached(Actor* actor);

	protected:

		FIELD()
		Actor* root = nullptr;

	private:

		b8 isPlaying = false;

		// - Cache -

		HashMap<Uuid, Actor*> actorsByUuid{};
		HashMap<TypeId, HashMap<Uuid, ActorComponent*>> componentsByType{};
		HashMap<MaterialInterface*, Array<MeshComponent*>> meshesByMaterial{};
		Array<CameraComponent*> cameras{};

		CameraComponent* mainCamera = nullptr;

		friend class Actor;
		friend class ActorComponent;
		friend class SceneComponent;
		friend class CameraComponent;
		friend class RendererSubsystem;
	};
    
} // namespace CE

#include "Scene.rtti.h"
