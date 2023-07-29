#include "System.h"

namespace CE
{
	void Engine::PreInit()
	{
		assetManager->Initialize();
	}

	void Engine::Initialize()
	{
		
	}

	void Engine::PreShutdown()
	{
		assetManager->Shutdown();
	}

	void Engine::Shutdown()
	{

	}

	void Engine::Tick(f32 deltaTime)
	{
		assetManager->Tick(deltaTime);
	}

} // namespace CE
