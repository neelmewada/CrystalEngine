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
			Actor* root = scene->GetRootActor();
			
			StaticMeshActor* meshActor = CreateObject<StaticMeshActor>(root, "StaticMeshActor");
			root->AttachActor(meshActor);

			StaticMeshComponent* meshComponent = meshActor->GetMeshComponent();
			
		}
	}

	void SandboxSubsystem::Shutdown()
	{
		Super::Shutdown();
	}

	void SandboxSubsystem::Tick(f32 delta)
	{
		Super::Tick(delta);
	}

} // namespace CE
