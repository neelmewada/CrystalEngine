
#include "GameFramework/Scene.h"

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

	}

} // namespace CE

CE_RTTI_CLASS_IMPL(SYSTEM_API, CE, Scene)

