#include "System.h"

namespace CE
{

	Scene::Scene()
	{
		root = CreateDefaultSubobject<Actor>("SceneRoot");
	}

	Scene::~Scene()
	{

	}

} // namespace CE
