#include "Sandbox.h"

namespace CE
{

	SandboxSubsystem::SandboxSubsystem()
	{
		
	}

	void SandboxSubsystem::Initialize()
	{
		Super::Initialize();

		auto sceneSubsystem = gEngine->GetSubsystem<SceneSubsystem>();
		if (sceneSubsystem != nullptr)
		{
			Scene* scene = sceneSubsystem->GetActiveScene();
			
		}
	}

	void SandboxSubsystem::Shutdown()
	{
		Super::Shutdown();
	}

} // namespace CE
