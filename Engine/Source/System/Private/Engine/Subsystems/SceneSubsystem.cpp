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
		scene = CreateObject<CE::Scene>(this, TEXT("EmptyScene"));
	}

	void SceneSubsystem::Shutdown()
	{
		if (scene)
		{
			scene->Destroy();
			scene = nullptr;
		}

		Super::Shutdown();
	}

	void SceneSubsystem::Tick(f32 deltaTime)
	{
		Super::Tick(deltaTime);

		if (!isPlaying)
		{
			isPlaying = true;

			if (scene != nullptr)
				scene->OnBeginPlay();
		}
		
		if (scene != nullptr)
		{
			scene->Tick(deltaTime);
		}
	}

} // namespace CE
