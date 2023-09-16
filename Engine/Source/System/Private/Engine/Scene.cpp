#include "System.h"

namespace CE
{

	Scene::Scene()
	{
		root = CreateDefaultSubobject<Node>("Root");
	}

	Scene::~Scene()
	{

	}

} // namespace CE
