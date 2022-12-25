
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
        // Clean up the object store
        for (auto [uuid, object] : objects)
        {
            delete object;
        }
        
        objects.Clear();
	}

    void Scene::Tick(f32 deltaTime)
    {
        for (int i = 0; i < gameObjects.GetSize(); i++)
        {
            if (gameObjects[i] != nullptr)
            {
                gameObjects[i]->Tick(deltaTime);
            }
        }
    }

    void Scene::AddGameObject(GameObject* gameObject)
    {
        gameObjects.Add(gameObject);
        objects.AddObject(gameObject);
        gameObject->owner = this;
        
        for (GameComponent* comp : gameObject->components)
        {
            objects.AddObject(comp);
        }
    }

    void Scene::DestroyGameObject(GameObject* gameObject)
    {
        gameObjects.Remove(gameObject);
        objects.RemoveObject(gameObject);
        gameObject->owner = nullptr;
        
        for (GameComponent* comp : gameObject->components)
        {
            objects.RemoveObject(comp);
        }
        
        delete gameObject;
    }

} // namespace CE

CE_RTTI_CLASS_IMPL(SYSTEM_API, CE, Scene)

