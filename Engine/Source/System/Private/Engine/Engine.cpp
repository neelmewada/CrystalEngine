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

		if (!mainThreadQueue.IsEmpty())
		{
			mainThreadQueue.GetFront().InvokeIfValid();
			mainThreadQueue.PopFront();
		}
	}

	void Engine::DispatchOnMainThread(Delegate<void(void)> action)
	{
		mainThreadQueueMutex.Lock();
		mainThreadQueue.PushBack(action);
		mainThreadQueueMutex.Unlock();
	}

} // namespace CE
