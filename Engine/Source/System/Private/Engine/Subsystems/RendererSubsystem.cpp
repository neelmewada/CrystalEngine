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

		auto renderTarget = gEngine->GetPrimaryGameViewport()->GetRenderTarget();

		auto errorShader = Shader::GetErrorShader();
		auto gpuShader = errorShader->FindOrCreateModule();

		RHI::ShaderResourceGroupDesc resourceGroup0Desc{};
		resourceGroup0Desc.variables.Add({
			.binding = 0,
			.name = "_PerViewData",
			.type = RHI::SHADER_RESOURCE_TYPE_CONSTANT_BUFFER,
			.isDynamic = false,
			.stageFlags = RHI::ShaderStage::Vertex
			});

		srg0 = RHI::gDynamicRHI->CreateShaderResourceGroup(resourceGroup0Desc);

		RHI::ShaderResourceGroupDesc emptyResourceGroupDesc{};
		srgEmpty = RHI::gDynamicRHI->CreateShaderResourceGroup(emptyResourceGroupDesc);

		srg0 = RHI::gDynamicRHI->CreateShaderResourceGroup(resourceGroup0Desc);

		RHI::ShaderResourceGroupDesc resourceGroup1Desc{};
		resourceGroup1Desc.variables.Add({
			.binding = 0,
			.name = "_Model",
			.type = RHI::SHADER_RESOURCE_TYPE_CONSTANT_BUFFER,
			.isDynamic = false,
			.stageFlags = RHI::ShaderStage::Vertex
			});

		srg1 = RHI::gDynamicRHI->CreateShaderResourceGroup(resourceGroup1Desc);

		RHI::GraphicsPipelineDesc desc = RHI::GraphicsPipelineBuilder()
			.VertexSize(sizeof(Vec3))
			.VertexAttrib(0, TYPEID(Vec3), 0)
			.CullMode(RHI::CULL_MODE_BACK)
			.VertexShader(gpuShader->vertex)
			.FragmentShader(gpuShader->fragment)
			.ShaderResource(srg0)
			.ShaderResource(srgEmpty)
			.ShaderResource(srg1)
			.Build();
        
		errorPipeline = RHI::gDynamicRHI->CreateGraphicsPipelineState(renderTarget, desc);
	}

	void RendererSubsystem::Shutdown()
	{
		if (srg0 != nullptr)
			RHI::gDynamicRHI->DestroyShaderResourceGroup(srg0);
		if (srgEmpty != nullptr)
			RHI::gDynamicRHI->DestroyShaderResourceGroup(srgEmpty);
		if (srg1 != nullptr)
			RHI::gDynamicRHI->DestroyShaderResourceGroup(srg1);
		if (errorPipeline != nullptr)
			RHI::gDynamicRHI->DestroyPipelineState(errorPipeline);
        errorPipeline = nullptr;

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

			StaticMesh* staticMesh = meshComponent->GetStaticMesh();
			if (staticMesh == nullptr)
				continue;

			auto vertexBuffer = staticMesh->GetErrorShaderVertexBuffer();

			Matrix4x4 modelMatrix = meshComponent->GetTransform();

			
		}
	}

} // namespace CE
