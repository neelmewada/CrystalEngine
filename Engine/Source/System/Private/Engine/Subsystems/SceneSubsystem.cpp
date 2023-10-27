#include "System.h"

namespace CE
{

    SceneSubsystem::SceneSubsystem()
    {
		
    }

	void SceneSubsystem::Initialize()
	{
		Super::Initialize();

		// Create & set an empty scene by default
		scene = CreateObject<Scene>(this, TEXT("EmptyScene"));
	}

	void SceneSubsystem::Shutdown()
	{
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
