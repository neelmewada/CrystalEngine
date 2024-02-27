#include "System.h"

namespace CE
{
	Array<ClassType*> Engine::subsystemClassQueue{};

	void Engine::PreInit()
	{
		
	}

	void Engine::Initialize()
	{
		for (auto classType : subsystemClassQueue)
		{
			CreateSubsystem(classType);
		}

		subsystemClassQueue.Clear();

		for (auto subsystem : engineSubsystems)
		{
			subsystem->Initialize();
		}

		isInitialized = true;
	}

	void Engine::PostInitialize()
	{
		for (auto subsystem : engineSubsystems)
		{
			subsystem->PostInitialize();
		}

		equirectShader = assetManager->LoadAssetAtPath<CE::Shader>("/Engine/Assets/Shaders/CubeMap/Equirectangular");
	}

	void Engine::PreShutdown()
	{
		equirectShader->Destroy();

		if (assetManager)
			assetManager->Shutdown();
		
		for (auto subsystem : engineSubsystems) // PreShutdown
			subsystem->PreShutdown();
	}

	void Engine::Shutdown()
	{
		isInitialized = false;

		// Shutdown Engine Subsystems
		for (auto subsystem : engineSubsystems) // Shutdown
			subsystem->Shutdown();
		for (auto subsystem : engineSubsystems) // Destroy
			subsystem->Destroy();
		engineSubsystems.Clear();
	}

	void Engine::Tick(f32 deltaTime)
	{
		if (assetManager)
			assetManager->Tick(deltaTime);

		for (auto subsystem : engineSubsystems)
		{
			subsystem->Tick(deltaTime);
		}

		if (!mainThreadQueue.IsEmpty())
		{
			mainThreadQueue.GetFront().InvokeIfValid();
			mainThreadQueue.PopFront();
		}

		for (auto gameInstance : gameInstances)
		{
			if (gameInstance->IsInitialized())
				gameInstance->Tick(deltaTime);
		}
	}

	void Engine::Render()
	{
		for (auto subsystem : engineSubsystems)
		{
			subsystem->Render();
		}
	}

	void Engine::DispatchOnMainThread(Delegate<void(void)> action)
	{
		mainThreadQueueMutex.Lock();
		mainThreadQueue.PushBack(action);
		mainThreadQueueMutex.Unlock();
	}

	GameInstance* Engine::GetGameInstance()
	{
		if (gameInstances.IsEmpty())
			return nullptr;
		return gameInstances.Top();
	}

	EngineSubsystem* Engine::GetSubsystem(ClassType* subsystemClass)
	{
		for (auto subsystem : engineSubsystems)
		{
			if (subsystem->IsOfType(subsystemClass))
			{
				return subsystem;
			}
		}

		return nullptr;
	}

	void Engine::SetPrimaryGameViewport(GameViewport* gameViewport)
	{
		this->primaryViewport = gameViewport;
	}

	EngineSubsystem* Engine::CreateSubsystem(ClassType* type)
	{
		if (type == nullptr || type->GetTypeId() == TYPEID(EngineSubsystem) || !type->IsSubclassOf<EngineSubsystem>())
			return nullptr;

		auto cdi = (EngineSubsystem*)type->GetDefaultInstance();
		if (cdi == nullptr || !cdi->ShouldBeCreated(this))
			return nullptr;

		// Check if subsystem already exists. Only 1 subsystem object allowed per class type.
		int index = engineSubsystems.IndexOf([&](EngineSubsystem* subsystem) -> bool
			{ return subsystem != nullptr && subsystem->GetType()->GetTypeId() == type->GetTypeId(); });
		
		if (index >= 0)
			return engineSubsystems[index];

		String name = type->GetName().GetLastComponent();
		EngineSubsystem* instance = CreateObject<EngineSubsystem>(this, name, OF_NoFlags, type);
		if (isInitialized)
		{
			instance->Initialize();
		}
		engineSubsystems.Add(instance);
		return instance;
	}

} // namespace CE
