#include "System.h"

namespace CE
{

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

		auto renderTarget = gEngine->GetPrimaryGameViewport()->GetRenderTarget();

		auto errorShader = Shader::GetErrorShader();
		auto shaderModules = errorShader->GetShaderModules();
		
		RHI::GraphicsPipelineDesc desc{};
		desc.vertexShader = shaderModules[0];
		desc.fragmentShader = shaderModules[1];
		desc.vertexSizeInBytes = sizeof(Vec3);
		desc.cullMode = RHI::CULL_MODE_BACK;
		
		desc.vertexAttribs.Resize(1);
		desc.vertexAttribs[0].dataType = TYPEID(Vec3);
		desc.vertexAttribs[0].location = 0;
		desc.vertexAttribs[0].offset = 0;

		errorPipeline = RHI::gDynamicRHI->CreateGraphicsPipelineState(renderTarget, desc);
	}

	void RendererSubsystem::Shutdown()
	{
		if (errorPipeline != nullptr)
			RHI::gDynamicRHI->DestroyPipelineState(errorPipeline);

		Super::Shutdown();
	}

	void RendererSubsystem::Tick(f32 delta)
	{
		Super::Tick(delta);

		if (sceneSubsystem == nullptr)
			return;
		
		auto scene = sceneSubsystem->GetActiveScene();
		if (scene == nullptr)
			return;

		CameraComponent* mainCamera = scene->GetMainCamera();
		if (mainCamera == nullptr)
			return;

		auto viewMatrix = mainCamera->GetTransform().GetInverse();
		
		const auto& components = scene->componentsByType[TYPEID(MeshComponent)];
		
		for (auto [uuid, component] : components)
		{
			MeshComponent* meshComponent = Object::CastTo<MeshComponent>(component);
			if (meshComponent == nullptr)
				continue;

			Matrix4x4 modelMatrix = meshComponent->GetTransform();

		}
	}

} // namespace CE
