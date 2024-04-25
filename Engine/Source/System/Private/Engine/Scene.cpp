#include "System.h"

namespace CE
{

	CE::Scene::Scene()
	{
		renderPipeline = CreateDefaultSubobject<MainRenderPipeline>("RenderPipeline");
		
		if (!IsDefaultInstance())
		{
			rpiScene = new RPI::Scene();
			rpiScene->AddFeatureProcessor<RPI::StaticMeshFeatureProcessor>();
			rpiScene->AddFeatureProcessor<RPI::DirectionalLightFeatureProcessor>();

			rpiScene->AddRenderPipeline(renderPipeline->GetRpiRenderPipeline());
		}
	}

	CE::Scene::~Scene()
	{
		delete rpiScene; rpiScene = nullptr;
	}

	void CE::Scene::OnBeginPlay()
	{
		isPlaying = true;

		for (Actor* actor : actors)
		{
			actor->OnBeginPlay();
		}
	}

	void CE::Scene::Tick(f32 delta)
	{
		for (Actor* actor : actors)
		{
			actor->Tick(delta);
		}
	}

	void CE::Scene::AddActor(Actor* actor)
	{
		if (actor == nullptr)
			return;
		if (actorsByUuid.KeyExists(actor->GetUuid()))
			return;

		actors.Add(actor);

		OnActorChainAttached(actor);
	}

	void CE::Scene::RemoveActor(Actor* actor)
	{
		if (actor == nullptr)
			return;
		if (!actorsByUuid.KeyExists(actor->GetUuid()))
			return;

		actors.Remove(actor);

		OnActorChainDetached(actor);
	}

	void CE::Scene::OnActorChainAttached(Actor* actor)
	{
		if (!actor)
			return;
		
        auto recursivelyAddSceneComponents = [&](SceneComponent* sceneComponent)
        {
            if (!sceneComponent)
                return;
			
            if (sceneComponent->IsOfType<CameraComponent>())
			{
				CameraComponent* camera = (CameraComponent*)sceneComponent;
				OnCameraComponentAttached(camera);
			}

			auto componentClass = sceneComponent->GetClass();
			auto componentUuid = sceneComponent->GetUuid();

			while (componentClass->GetTypeId() != TYPEID(Object))
			{
				componentsByType[componentClass->GetTypeId()][componentUuid] = sceneComponent;

				componentClass = componentClass->GetSuperClass(0);
			}
            
            for (auto component : sceneComponent->attachedComponents)
            {
                if (component != nullptr && component != sceneComponent)
                    recursivelyAddSceneComponents(component);
            }
        };

		auto recursivelyAdd = [&](Actor* add)
        {
            if (!add)
                return;

			add->scene = this;
            
            if (add->rootComponent != nullptr)
                recursivelyAddSceneComponents(add->rootComponent);
            
			actorsByUuid[add->GetUuid()] = add;

			for (auto component : add->attachedComponents)
			{
				auto componentClass = component->GetClass();

				while (componentClass->GetTypeId() != TYPEID(Object))
				{
					componentsByType[componentClass->GetTypeId()][component->GetUuid()] = component;

					componentClass = componentClass->GetSuperClass(0);
				}
			}

            for (auto child : add->children)
            {
                if (child != nullptr && child != add)
                    recursivelyAdd(child);
            }
        };

		recursivelyAdd(actor);

		if (isPlaying && !actor->hasBegunPlaying)
		{
			actor->OnBeginPlay();
		}
	}

	void CE::Scene::OnActorChainDetached(Actor* actor)
	{
		if (!actor)
			return;
        
        auto recursivelyRemoveSceneComponents = [&](SceneComponent* sceneComponent)
        {
            if (!sceneComponent)
                return;
            
            if (sceneComponent->IsOfType<CameraComponent>())
			{
				CameraComponent* camera = (CameraComponent*)sceneComponent;
				OnCameraComponentDetached(camera);
			}

			auto componentClass = sceneComponent->GetClass();
			auto componentUuid = sceneComponent->GetUuid();

			while (componentClass->GetTypeId() != TYPEID(Object))
			{
				componentsByType[componentClass->GetTypeId()].Remove(componentUuid);

				componentClass = componentClass->GetSuperClass(0);
			}
            
            for (auto component : sceneComponent->attachedComponents)
            {
                if (component != nullptr && component != sceneComponent)
                    recursivelyRemoveSceneComponents(component);
            }
        };

		auto recursivelyRemove = [&](Actor* remove)
        {
            if (!remove)
                return;
            
            if (remove->rootComponent != nullptr)
                recursivelyRemoveSceneComponents(remove->rootComponent);
            
			actorsByUuid.Remove(remove->GetUuid());

			for (auto component : remove->attachedComponents)
			{
				auto componentClass = component->GetClass();
				auto componentUuid = component->GetUuid();

				while (componentClass->GetTypeId() != TYPEID(Object))
				{
					componentsByType[componentClass->GetTypeId()].Remove(componentUuid);

					componentClass = componentClass->GetSuperClass(0);
				}
			}

			remove->scene = nullptr;

            for (auto child : remove->children)
            {
                if (child != nullptr && child != remove)
                    recursivelyRemove(child);
            }
        };

		recursivelyRemove(actor);
	}

	void CE::Scene::OnCameraComponentAttached(CameraComponent* camera)
	{
		cameras.Add(camera);
		if (mainCamera == nullptr)
		{
			mainCamera = camera;
			rpiScene->AddView("MainCamera", mainCamera->rpiView);
		}
		else
		{
			rpiScene->AddView("Camera", camera->rpiView);
		}
	}

	void CE::Scene::OnCameraComponentDetached(CameraComponent* camera)
	{
		cameras.Remove(camera);
		if (mainCamera == camera)
		{
			rpiScene->RemoveView("MainCamera", mainCamera->rpiView);
			mainCamera = cameras.IsEmpty() ? nullptr : cameras.Top();
		}
		else
		{
			rpiScene->RemoveView("Camera", camera->rpiView);
		}
	}

	void CE::Scene::OnRootComponentSet(SceneComponent* rootComponent, Actor* ownerActor)
	{

	}

} // namespace CE
