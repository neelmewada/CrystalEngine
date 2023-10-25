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

} // namespace CE
