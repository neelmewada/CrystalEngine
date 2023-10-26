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

} // namespace CE
