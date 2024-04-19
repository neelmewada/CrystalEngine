#include "CoreRPI.h"

namespace CE::RPI
{
    
	Scene::Scene()
	{
		RPISystem::Get().scenes.Add(this);
	}

	Scene::~Scene()
	{
		RPISystem::Get().scenes.Remove(this);
	}

} // namespace CE::RPI
