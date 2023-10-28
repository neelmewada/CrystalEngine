#include "System.h"

namespace CE
{

    SceneSubsystem::SceneSubsystem()
    {
		
    }

	void SceneSubsystem::OnBeginPlay()
	{
		if (scene != nullptr)
			scene->OnBeginPlay();
	}

	void SceneSubsystem::Initialize()
	{
		Super::Initialize();

		// Create & set an empty scene by default
		scene = CreateObject<Scene>(this, TEXT("EmptyScene"));
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
		
		if (scene != nullptr)
		{
			scene->Tick(deltaTime);
		}
	}

} // namespace CE
