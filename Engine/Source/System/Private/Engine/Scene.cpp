#include "System.h"

namespace CE
{

	Scene::Scene()
	{
		root = CreateDefaultSubobject<Actor>("SceneRoot");
		root->owner = this;
	}

	Scene::~Scene()
	{

	}

} // namespace CE
