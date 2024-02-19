#include "System.h"

namespace CE
{
	RHI::Buffer* vertexBuffer = nullptr;

    RendererSubsystem::RendererSubsystem()
    {
		
    }

	void RendererSubsystem::Initialize()
	{
		Super::Initialize();
	}

	void RendererSubsystem::PostInitialize()
	{
		Super::PostInitialize();

		sceneSubsystem = gEngine->GetSubsystem<SceneSubsystem>();

		
	}

	void RendererSubsystem::Shutdown()
	{
		if (srg0 != nullptr)
			RHI::gDynamicRHI->DestroyShaderResourceGroup(srg0);
		if (srgEmpty != nullptr)
			RHI::gDynamicRHI->DestroyShaderResourceGroup(srgEmpty);
		if (srg1 != nullptr)
			RHI::gDynamicRHI->DestroyShaderResourceGroup(srg1);

		Super::Shutdown();
	}

	void RendererSubsystem::Tick(f32 delta)
	{
		Super::Tick(delta);
		
	}

	void RendererSubsystem::Render()
	{
		if (sceneSubsystem == nullptr)
			return;

		auto scene = sceneSubsystem->GetActiveScene();
		if (scene == nullptr)
			return;

		CameraComponent* mainCamera = scene->GetMainCamera();
		if (mainCamera == nullptr)
			return;

		auto cameraMatrix = mainCamera->GetTransform();

		auto viewMatrix = cameraMatrix.GetInverse();

		const auto& components = scene->componentsByType[TYPEID(MeshComponent)];

		for (auto [uuid, component] : components)
		{
			StaticMeshComponent* meshComponent = Object::CastTo<StaticMeshComponent>(component);
			if (meshComponent == nullptr)
				continue;

			
		}
	}

} // namespace CE
