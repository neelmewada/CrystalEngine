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

		auto renderTarget = gEngine->GetPrimaryRenderTarget();

		auto errorShader = Shader::GetErrorShader();
		auto shaderModules = errorShader->GetShaderModules();
		
		RHI::GraphicsPipelineDesc desc{};
		desc.vertexShader = shaderModules[0];
		desc.fragmentShader = shaderModules[1];
		desc.vertexSizeInBytes = sizeof(Vec3);
		desc.cullMode = RHI::CULL_MODE_BACK;
		
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


	}

} // namespace CE
