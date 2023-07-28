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

} // namespace CE
