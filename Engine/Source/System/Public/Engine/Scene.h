#pragma once
#include "GameFramework/ActorComponent.h"

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
        
		virtual void OnBeginPlay();

		virtual void Tick(f32 delta);

		CameraComponent* GetMainCamera() const { return mainCamera; }

		RPI::Scene* GetRpiScene() const { return rpiScene; }

		void AddActor(Actor* actor);
		void RemoveActor(Actor* actor);

		void IterateAllComponents(SubClass<ActorComponent> componentClass, auto callback)
		{
			if (componentClass == nullptr)
				return;

			TypeId componentTypeId = componentClass->GetTypeId();

			if (!componentsByType.KeyExists(componentTypeId))
				return;

			for (auto [uuid, component] : componentsByType[componentTypeId])
			{
				callback(component);
			}
		}

		template<typename TActorComponent> requires TIsBaseClassOf<ActorComponent, TActorComponent>::Value
		void IterateAllComponents(auto callback)
		{
			ClassType* clazz = TActorComponent::StaticType();
			if (clazz == nullptr)
				return;

			TypeId componentTypeId = clazz->GetTypeId();

			if (!componentsByType.KeyExists(componentTypeId))
				return;

			for (auto [uuid, component] : componentsByType[componentTypeId])
			{
				if (component->IsOfType(clazz))
				{
					callback((TActorComponent*)component);
				}
			}
		}

	private:

		// - Internal API -

		void OnActorChainAttached(Actor* actor);
		void OnActorChainDetached(Actor* actor);

		void OnCameraComponentAttached(CameraComponent* cameraComponent);
		void OnCameraComponentDetached(CameraComponent* cameraComponent);

		void OnRootComponentSet(SceneComponent* rootComponent, Actor* ownerActor);

	protected:

		FIELD()
		Array<Actor*> actors{};

	private:

		b8 isPlaying = false;

		// - RPI -

		RPI::Scene* rpiScene = nullptr;

		// - Cache -

		HashMap<Uuid, Actor*> actorsByUuid{};
		HashMap<TypeId, HashMap<Uuid, ActorComponent*>> componentsByType{};
		Array<CameraComponent*> cameras{};

		CameraComponent* mainCamera = nullptr;

		friend class Actor;
		friend class ActorComponent;
		friend class SceneComponent;
		friend class CameraComponent;
		friend class RendererSubsystem;
		friend class SceneSubsystem;
	};
    
} // namespace CE

#include "Scene.rtti.h"
