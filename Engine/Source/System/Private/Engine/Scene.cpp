#include "System.h"

namespace CE
{

	CE::Scene::Scene()
	{
		root = CreateDefaultSubobject<Actor>("SceneRoot");
		root->scene = this;
	}

	CE::Scene::~Scene()
	{
        
	}

	void CE::Scene::OnBeginPlay()
	{
		isPlaying = true;

		if (root != nullptr)
		{
			root->OnBeginPlay();
		}
	}

	void CE::Scene::Tick(f32 delta)
	{
		if (root != nullptr)
		{
			root->Tick(delta);
		}
	}

	void CE::Scene::OnActorChainAttached(Actor* actor)
	{
		if (!actor)
			return;
		
        std::function<void(SceneComponent*)> recursivelyAddSceneComponents = [&](SceneComponent* sceneComponent)
        {
            if (!sceneComponent)
                return;
			
            if (sceneComponent->IsOfType<CameraComponent>())
			{
				CameraComponent* camera = (CameraComponent*)sceneComponent;
				cameras.Add(camera);
				if (camera->IsMainCamera())
					mainCamera = camera;
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

		std::function<void(Actor*)> recursivelyAdd = [&](Actor* add)
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
        
        std::function<void(SceneComponent*)> recursivelyRemoveSceneComponents = [&](SceneComponent* sceneComponent)
        {
            if (!sceneComponent)
                return;
            
            if (sceneComponent->IsOfType<CameraComponent>())
			{
				cameras.Remove((CameraComponent*)sceneComponent);
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

		std::function<void(Actor*)> recursivelyRemove = [&](Actor* remove)
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

} // namespace CE
