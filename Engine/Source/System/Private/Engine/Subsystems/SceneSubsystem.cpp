#include "System.h"

namespace CE
{

    SceneSubsystem::SceneSubsystem()
    {
		
    }

	void SceneSubsystem::Initialize()
	{
		Super::Initialize();
	}

	void SceneSubsystem::PostInitialize()
	{
		Super::PostInitialize();

		// Create & set an empty scene by default
		mainScene = CreateObject<CE::Scene>(this, TEXT("EmptyScene"));
	}

	void SceneSubsystem::PreShutdown()
	{
		if (mainScene)
		{
			mainScene->Destroy();
			mainScene = nullptr;
		}

		Super::PreShutdown();
	}

	void SceneSubsystem::Shutdown()
	{

		Super::Shutdown();
	}

	void SceneSubsystem::Tick(f32 deltaTime)
	{
		Super::Tick(deltaTime);

		if (!isPlaying)
		{
			isPlaying = true;

			if (mainScene != nullptr)
				mainScene->OnBeginPlay();
		}
		
		if (mainScene != nullptr)
		{
			mainScene->Tick(deltaTime);
		}
	}

} // namespace CE
