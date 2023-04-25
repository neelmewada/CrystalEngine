
#include "GameFramework/Scene.h"

#include "GameFramework/GameObject.h"

namespace CE
{

	Scene::Scene() : Object()
	{
        
	}

	Scene::Scene(Name name) : Object(name)
	{
		
	}

	Scene::~Scene()
	{
        for (auto gameObject : rootGameObjects)
        {
            delete gameObject;
        }

        rootGameObjects.Clear();
	}

    void Scene::Tick(f32 deltaTime)
    {
        for (int i = 0; i < rootGameObjects.GetSize(); i++)
        {
            if (rootGameObjects[i] != nullptr)
            {
                rootGameObjects[i]->Tick(deltaTime);
            }
        }
    }

    void Scene::AddGameObject(GameObject* gameObject)
    {
        rootGameObjects.Add(gameObject);
        objects.AddObject(gameObject);
        gameObject->owner = this;
        
        for (GameComponent* comp : gameObject->components)
        {
            objects.AddObject(comp);
        }
    }

    void Scene::RemoveGameObject(GameObject* gameObject)
    {
        rootGameObjects.Remove(gameObject);
        objects.RemoveObject(gameObject);

        for (GameComponent* comp : gameObject->components)
        {
            objects.RemoveObject(comp);
        }
    }

    void Scene::DestroyGameObject(GameObject* gameObject)
    {
        if (gameObject == nullptr)
            return;

        RemoveGameObject(gameObject);
        
        delete gameObject;
    }

    void Scene::AddObject(Object* object)
    {
        objects.AddObject(object);
        
        auto typeId = object->GetType()->GetTypeId();

        if (objectTypeToArrayMap.KeyExists(typeId))
        {
            objectTypeToArrayMap[typeId].Add(object);
        }
        else
        {
            objectTypeToArrayMap.Add({ typeId, { object } });
        }
    }

    void Scene::RemoveObject(Object* object)
    {
        objects.RemoveObject(object);

        auto typeId = object->GetType()->GetTypeId();

        if (objectTypeToArrayMap.KeyExists(typeId))
        {
            objectTypeToArrayMap[typeId].Remove(object);
        }
    }

    void Scene::DestroyObject(Object* object)
    {
        RemoveObject(object);

        delete object;
    }

    void Scene::DestroyAll()
    {
        objects.DestroyAll();
        rootGameObjects.Clear();
    }

} // namespace CE
