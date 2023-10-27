#include "System.h"

namespace CE
{

	Scene::Scene()
	{
		root = CreateDefaultSubobject<Actor>("SceneRoot");
		root->scene = this;
	}

	Scene::~Scene()
	{
        
	}

	void Scene::Tick(f32 delta)
	{
		if (root != nullptr)
		{
			root->Tick(delta);
		}
	}

	void Scene::OnActorChainAttached(Actor* actor)
	{
		if (!actor)
			return;
        
        std::function<void(SceneComponent*)> recursivelyAddSceneComponents = [&](SceneComponent* sceneComponent)
        {
            if (!sceneComponent)
                return;
            
            if (sceneComponent->IsOfType<CameraComponent>())
                camerasByUuid[sceneComponent->GetUuid()] = (CameraComponent*)sceneComponent;
            else if (sceneComponent->IsOfType<StaticMeshComponent>())
                staticMeshComponentsByUuid[sceneComponent->GetUuid()] = (StaticMeshComponent*)sceneComponent;
            
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
            
            if (add->rootComponent != nullptr)
                recursivelyAddSceneComponents(add->rootComponent);
            
            actorInstancesByUuid[add->GetUuid()] = add;

            for (auto child : add->children)
            {
                if (child != nullptr && child != add)
                    recursivelyAdd(child);
            }
        };

		recursivelyAdd(actor);
	}

	void Scene::OnActorChainDetached(Actor* actor)
	{
		if (!actor)
			return;
        
        std::function<void(SceneComponent*)> recursivelyRemoveSceneComponents = [&](SceneComponent* sceneComponent)
        {
            if (!sceneComponent)
                return;
            
            if (sceneComponent->IsOfType<CameraComponent>())
                camerasByUuid.Remove(sceneComponent->GetUuid());
            else if (sceneComponent->IsOfType<StaticMeshComponent>())
                staticMeshComponentsByUuid.Remove(sceneComponent->GetUuid());
            
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
            
            actorInstancesByUuid.Remove(remove->GetUuid());

            for (auto child : remove->children)
            {
                if (child != nullptr && child != remove)
                    recursivelyRemove(child);
            }
        };

		recursivelyRemove(actor);
	}

} // namespace CE
