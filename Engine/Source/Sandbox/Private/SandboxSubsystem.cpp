#include "Sandbox.h"

namespace CE
{

	SandboxSubsystem::SandboxSubsystem()
	{
		
	}

	void SandboxSubsystem::SetupScene(Scene* scene)
	{
		Actor* root = scene->GetRootActor();

		StaticMeshActor* meshActor = CreateObject<StaticMeshActor>(root, "StaticMeshActor");
		{
			StaticMeshComponent* meshComponent = meshActor->GetMeshComponent();
			StaticMesh* cubeStaticMesh = StaticMesh::GetCubeMesh();
			meshComponent->SetStaticMesh(cubeStaticMesh);
			meshComponent->SetLocalPosition(Vec3(0, 0, 10));
		}
		root->AttachActor(meshActor);

		CameraActor* camera = CreateObject<CameraActor>(root, "CameraActor");
		{
			CameraComponent* cameraComponent = camera->GetCameraComponent();
			cameraComponent->SetLocalPosition(Vec3(0, 0, 0));
			cameraComponent->SetProjection(CameraProjection::Perspective);
			cameraComponent->SetMainCamera();
		}
		root->AttachActor(camera);
	}

	void SandboxSubsystem::Initialize()
	{
		Super::Initialize();
        
		auto sceneSubsystem = gEngine->GetSubsystem<SceneSubsystem>();
		if (sceneSubsystem != nullptr)
		{
			Scene* scene = sceneSubsystem->GetActiveScene();
			SetupScene(scene);
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
