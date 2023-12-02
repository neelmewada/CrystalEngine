#include "GameLauncher.h"

GameLoop gGameLoop{};

void GameLoop::PreInit(int argc, char** argv)
{
	// Setup before loading anything

	gProjectPath = PlatformDirectories::GetLaunchDir();
	String folderName = gProjectPath.GetFilename().GetString();

	if (folderName != "Debug" && folderName != "Development" && folderName != "Profile" && folderName != "Release")
	{
		// Running inside final runtime build
		gProjectName = folderName;
	}
	else
	{
		// Running from build output folder (in sandbox testing environment)
		gProjectName = "CrystalEngine.Standalone";
	}

	if (gDefaultWindowWidth == 0)
		gDefaultWindowWidth = 1280;
	if (gDefaultWindowHeight == 0)
		gDefaultWindowHeight = 720;

	// Initialize logging
	Logger::Initialize();
	Logger::SetConsoleLogLevel(LogLevel::Trace);
	Logger::SetFileDumpLogLevel(LogLevel::Trace);

	gProgramArguments.Clear();
	for (int i = 0; i < argc; i++)
	{
		gProgramArguments.Add(argv[i]);
	}

	cxxopts::Options options = cxxopts::Options(gProjectName.ToStdString(), (gProjectName + " command line arguments.").ToStdString());

	options.add_options()
		("h,help", "Print this help info and exit")
		;

	options.allow_unrecognised_options();

	try
	{
		cxxopts::ParseResult result = options.parse(argc, argv);

		if (result["h"].as<bool>())
		{
			std::cout << options.help() << std::endl;
			exit(0);
			return;
		}


	}
	catch (std::exception exc)
	{
		CE_LOG(Error, All, "Failed to parse arguments: {}", exc.what());
		exit(-1);
	}
}

void GameLoop::LoadStartupCoreModules()
{
	// Load Startup Modules
	ModuleManager::Get().LoadModule("Core");
	ModuleManager::Get().LoadModule("CoreApplication");

	// Settings module
	ModuleManager::Get().LoadModule("CoreSettings");

	// Load Rendering modules
	ModuleManager::Get().LoadModule("CoreRHI");
#if PAL_TRAIT_VULKAN_SUPPORTED
	ModuleManager::Get().LoadModule("VulkanRHI");
#else
#	error Vulkan API is required but not supported by current configuration
#endif
}

void GameLoop::LoadCoreModules()
{
	// Load other Core modules
	ModuleManager::Get().LoadModule("CoreMedia");
}

void GameLoop::LoadEngineModules()
{
	ModuleManager::Get().LoadModule("System");
	ModuleManager::Get().LoadModule("GameSystem");
}

void GameLoop::UnloadEngineModules()
{
	ModuleManager::Get().UnloadModule("GameSystem");
	ModuleManager::Get().UnloadModule("System");
}

void GameLoop::Init()
{
	// Load most important core modules for startup
	LoadStartupCoreModules();

	// Load Project
	LoadProject();

	// Load application
	AppPreInit();
}

void GameLoop::PostInit()
{
	// Load non-important core modules
	LoadCoreModules();

	RHI::gDynamicRHI = new VulkanRHI();
	RHI::gDynamicRHI->Initialize();

	AppInit();

	RHI::gDynamicRHI->PostInitialize();

	// Load game engine modules
	LoadEngineModules();

	gEngine->PreInit();

	// Load Render viewports & cmd lists

	u32 width = 0, height = 0;
	mainWindow->GetDrawableWindowSize(&width, &height);

	RHI::RenderTargetColorOutputDesc colorDesc{};
	colorDesc.loadAction = RHI::RenderPassLoadAction::Clear;
	colorDesc.storeAction = RHI::RenderPassStoreAction::Store;
	colorDesc.sampleCount = 1;
	colorDesc.preferredFormat = RHI::ColorFormat::Auto;

	RHI::RenderTargetLayout rtLayout{};
	rtLayout.backBufferCount = 2;
	rtLayout.numColorOutputs = 1;
	rtLayout.colorOutputs[0] = colorDesc;
	rtLayout.presentationRTIndex = 0;
	rtLayout.depthStencilFormat = RHI::DepthStencilFormat::Auto;

	viewport = RHI::gDynamicRHI->CreateViewport(mainWindow, width, height, false, rtLayout);

	// Create GameViewport
	GameViewport* gameViewport = CreateObject<GameViewport>(gEngine, "GameViewport");
	gameViewport->Initialize(viewport->GetRenderTarget());

	cmdList = RHI::gDynamicRHI->CreateGraphicsCommandList(viewport);
	
	// Load game code
	ModuleManager::Get().LoadModule("Sandbox");

	// Initialize engine & it's subsystems, and set game viewport
	gEngine->Initialize();

	gEngine->SetPrimaryGameViewport(gameViewport);

	// Post initialize
	gEngine->PostInitialize();
}

void GameLoop::RunLoop()
{
	SceneSubsystem* sceneSubsystem = gEngine->GetSubsystem<SceneSubsystem>();

	if (sceneSubsystem)
		sceneSubsystem->OnBeginPlay();

	// TODO: Test Code

	auto renderTarget = gEngine->GetPrimaryGameViewport()->GetRenderTarget();

	auto errorShader = Shader::GetErrorShader();
	auto gpuShaderModule = errorShader->FindOrCreateModule();

	RHI::ShaderResourceGroupDesc resourceGroup0Desc{};
	resourceGroup0Desc.variables.Add({
		.binding = 0,
		.name = "_PerViewData",
		.type = RHI::SHADER_RESOURCE_TYPE_CONSTANT_BUFFER,
		.isDynamic = false,
		.stageFlags = RHI::ShaderStage::Vertex
		});

	auto srg0 = RHI::gDynamicRHI->CreateShaderResourceGroup(resourceGroup0Desc);

	RHI::ShaderResourceGroupDesc emptyResourceGroupDesc{};
	auto srgEmpty = RHI::gDynamicRHI->CreateShaderResourceGroup(emptyResourceGroupDesc);

	RHI::ShaderResourceGroupDesc resourceGroup1Desc{};
	resourceGroup1Desc.variables.Add({
		.binding = 0,
		.name = "_Model",
		.type = RHI::SHADER_RESOURCE_TYPE_CONSTANT_BUFFER,
		.isDynamic = false,
		.stageFlags = RHI::ShaderStage::Vertex
		});

	auto srg1 = RHI::gDynamicRHI->CreateShaderResourceGroup(resourceGroup1Desc);

	RHI::GraphicsPipelineDesc desc = RHI::GraphicsPipelineBuilder()
		.VertexSize(sizeof(Vec3))
		.VertexAttrib(0, TYPEID(Vec3), 0)
		.CullMode(RHI::CULL_MODE_NONE)
		.VertexShader(gpuShaderModule->vertex)
		.FragmentShader(gpuShaderModule->fragment)
		.ShaderResource(srg0)
		.ShaderResource(srgEmpty)
		.ShaderResource(srg1)
		.Build();

	auto errorPipeline = RHI::gDynamicRHI->CreateGraphicsPipelineState(renderTarget, desc);

	StaticMesh* cubeMesh = StaticMesh::GetCubeMesh();
	ModelData* cubeModel = cubeMesh->GetModelData();
	RHI::Buffer* vertBuffer = cubeMesh->GetErrorShaderVertexBuffer();
	RHI::Buffer* indexBuffer = cubeMesh->GetIndexBuffer();
	u32 numIndices = cubeModel->lod[0]->subMeshes[0].indices.GetSize();

	Matrix4x4 modelMatrix{};
	Vec3 localPos = Vec3(0, 0, 10);// 0.5f);
	Vec3 localEuler = Vec3(0, 0, 0);
	Vec3 localScale = Vec3(1, 1, 1);
	
	{
		Matrix4x4 translation = Matrix4x4::Identity();
		translation[0][3] = localPos.x;
		translation[1][3] = localPos.y;
		translation[2][3] = localPos.z;

		Quat localRotation = Quat::EulerDegrees(localEuler);
		Matrix4x4 rotation = localRotation.ToMatrix();

		Matrix4x4 scale = Matrix4x4::Identity();
		scale[0][0] = localScale.x;
		scale[1][1] = localScale.y;
		scale[2][2] = localScale.z;

		modelMatrix = translation * rotation * scale;
	}

	Matrix4x4 viewMatrix{};
	Vec3 cameraPos = Vec3(0, 0, 0);
	Vec3 cameraEuler = Vec3(0, 0, 0);
	Vec3 cameraScale = Vec3(1, 1, 1);

	{
		Matrix4x4 translation = Matrix4x4::Identity();
		translation[0][3] = cameraPos.x;
		translation[1][3] = cameraPos.y;
		translation[2][3] = cameraPos.z;

		Quat localRotation = Quat::EulerDegrees(cameraEuler);
		Matrix4x4 rotation = localRotation.ToMatrix();

		Matrix4x4 scale = Matrix4x4::Identity();
		scale[0][0] = cameraScale.x;
		scale[1][1] = cameraScale.y;
		scale[2][2] = cameraScale.z;

		viewMatrix = (translation * rotation * scale).GetInverse();
	}

	Matrix4x4 projectionMatrix{};

	auto rt = viewport->GetRenderTarget();

	{
		float fov = 50.0f;  // Field of view in degrees
		float n = 0.1f, f = 500.0f;
		float w = rt->GetWidth();
		float h = rt->GetHeight();
		float aspect = w / h;

		projectionMatrix = Matrix4x4::PerspectiveProjection(aspect, fov, n, f);
	}

	struct PerViewData
	{
		Matrix4x4 viewMatrix;
		Matrix4x4 viewProjectionMatrix;
		Matrix4x4 projectionMatrix;
	} perViewUniforms;

	perViewUniforms.viewMatrix = viewMatrix;
	perViewUniforms.viewProjectionMatrix = projectionMatrix * viewMatrix;
	perViewUniforms.projectionMatrix = projectionMatrix;

	RHI::Buffer* perViewBuffer = nullptr;
	{
		RHI::BufferData initialData{};
		initialData.data = &perViewUniforms;
		initialData.dataSize = sizeof(perViewUniforms);
		initialData.startOffsetInBuffer = 0;

		RHI::BufferDesc bufferDesc{};
		bufferDesc.bindFlags = RHI::BufferBindFlags::ConstantBuffer;
		bufferDesc.allocMode = RHI::BufferAllocMode::SharedMemory;
		bufferDesc.usageFlags = RHI::BufferUsageFlags::Default;
		bufferDesc.name = "PerView Uniforms";
		bufferDesc.bufferSize = sizeof(perViewUniforms);
		bufferDesc.structureByteStride = bufferDesc.bufferSize;
		bufferDesc.initialData = &initialData;

		perViewBuffer = RHI::gDynamicRHI->CreateBuffer(bufferDesc);
	}

	RHI::Buffer* perModelBuffer = nullptr;
	{
		RHI::BufferData initialData{};
		initialData.data = &modelMatrix;
		initialData.dataSize = sizeof(modelMatrix);
		initialData.startOffsetInBuffer = 0;

		RHI::BufferDesc bufferDesc{};
		bufferDesc.bindFlags = RHI::BufferBindFlags::ConstantBuffer;
		bufferDesc.allocMode = RHI::BufferAllocMode::SharedMemory;
		bufferDesc.usageFlags = RHI::BufferUsageFlags::Default;
		bufferDesc.name = "PerModel Uniforms";
		bufferDesc.bufferSize = sizeof(modelMatrix);
		bufferDesc.structureByteStride = bufferDesc.bufferSize;
		bufferDesc.initialData = &initialData;

		perModelBuffer = RHI::gDynamicRHI->CreateBuffer(bufferDesc);
	}

	RHI::IPipelineLayout* pipelineLayout = errorPipeline->GetPipelineLayout();

	srg0->Bind("_PerViewData", perViewBuffer);
	srg1->Bind("_Model", perModelBuffer);

	float curAspect = (float)rt->GetWidth() / (float)rt->GetHeight();

	f32 rot = 0;
	
	while (!IsEngineRequestingExit())
	{
		auto curTime = clock();
		f32 deltaTime = ((f32)(curTime - previousTime)) / CLOCKS_PER_SEC;

		app->Tick();
		
		// - Engine -
		gEngine->Tick(deltaTime);

		if (!sceneSubsystem)
			sceneSubsystem = gEngine->GetSubsystem<SceneSubsystem>();

		rot += deltaTime * 100;
		if (rot > 360)
			rot = 0;

		// - Update Model Matrix -
		{
			Matrix4x4 translation = Matrix4x4::Identity();
			translation[0][3] = localPos.x;
			translation[1][3] = localPos.y;
			translation[2][3] = localPos.z;

			localEuler.y = rot;
			Quat localRotation = Quat::EulerDegrees(localEuler);
			Matrix4x4 rotation = localRotation.ToMatrix();

			Matrix4x4 scale = Matrix4x4::Identity();
			scale[0][0] = localScale.x;
			scale[1][1] = localScale.y;
			scale[2][2] = localScale.z;

			modelMatrix = translation * rotation * scale;

			RHI::BufferData modelData{};
			modelData.data = &modelMatrix;
			modelData.dataSize = sizeof(modelMatrix);
			modelData.startOffsetInBuffer = 0;
			perModelBuffer->UploadData(modelData);
		}

		// - Update Projection -
		float w = rt->GetWidth();
		float h = rt->GetHeight();
		float aspect = w / h;

		if (curAspect != aspect)
		{
			curAspect = aspect;
			float fov = 50.0f;  // Field of view in degrees
			float n = 0.1f, f = 500.0f;

			// Perspective
			projectionMatrix = Matrix4x4::PerspectiveProjection(aspect, fov, n, f);
			perViewUniforms.viewProjectionMatrix = projectionMatrix * viewMatrix;
			perViewUniforms.projectionMatrix = projectionMatrix;

			RHI::BufferData perViewBufferData{};
			perViewBufferData.data = &perViewUniforms;
			perViewBufferData.dataSize = sizeof(perViewUniforms);
			perViewBufferData.startOffsetInBuffer = 0;

			perViewBuffer->UploadData(perViewBufferData);
		}

		// - Render -
		viewport->SetClearColor(Color::Black());
		if (sceneSubsystem && sceneSubsystem->GetActiveScene() != nullptr)
		{
			CameraComponent* mainCamera = sceneSubsystem->GetActiveScene()->GetMainCamera();
			if (mainCamera != nullptr)
			{
				viewport->SetClearColor(mainCamera->GetClearColor());
			}
		}

		cmdList->Begin();

		cmdList->BindPipeline(errorPipeline);

		cmdList->BindVertexBuffers(0, { vertBuffer });
		cmdList->BindIndexBuffer(indexBuffer, false, 0);
		
		cmdList->CommitShaderResources(0, { srg0, srgEmpty, srg1 }, pipelineLayout);

		cmdList->DrawIndexed(numIndices, 1, 0, 0, 0);
		
		cmdList->End();

		if (RHI::gDynamicRHI->ExecuteCommandList(cmdList))
		{
			RHI::gDynamicRHI->PresentViewport(cmdList);
		}
		
		previousTime = curTime;
	}

	cmdList->WaitForExecution();

	RHI::gDynamicRHI->DestroyBuffer(perViewBuffer);
	RHI::gDynamicRHI->DestroyBuffer(perModelBuffer);

	RHI::gDynamicRHI->DestroyShaderResourceGroup(srgEmpty);

	RHI::gDynamicRHI->DestroyPipelineState(errorPipeline);

	RHI::gDynamicRHI->DestroyShaderResourceGroup(srg0);
	RHI::gDynamicRHI->DestroyShaderResourceGroup(srg1);
}

void GameLoop::PreShutdown()
{
	// Unload game code

	UnloadSettings();

	gEngine->PreShutdown();

	gEngine->GetPrimaryGameViewport()->Shutdown();

	gEngine->Shutdown();

	RHI::gDynamicRHI->DestroyCommandList(cmdList);
	RHI::gDynamicRHI->DestroyViewport(viewport);

	ModuleManager::Get().UnloadModule("Sandbox");
	UnloadEngineModules();

	RHI::gDynamicRHI->PreShutdown();

	AppPreShutdown();

	RHI::gDynamicRHI->Shutdown();

	delete RHI::gDynamicRHI;
	RHI::gDynamicRHI = nullptr;

	ModuleManager::Get().UnloadModule("VulkanRHI");
	ModuleManager::Get().UnloadModule("CoreRHI");
}

void GameLoop::Shutdown()
{
	// Shutdown application
	AppShutdown();

	// Unload settings module
	ModuleManager::Get().UnloadModule("CoreSettings");

	// Unload most important modules at last
	ModuleManager::Get().UnloadModule("CoreApplication");
	ModuleManager::Get().UnloadModule("Core");

	Logger::Shutdown();
}

void GameLoop::LoadProject()
{
	// Load project (aka Setings)
	Package* settingsPackage = GetSettingsPackage();

	ProjectSettings* projectSettings = GetSettings<ProjectSettings>();
	if (projectSettings != nullptr)
	{
		gProjectName = projectSettings->projectName;
	}
}

void GameLoop::UnloadProject()
{
	
}

void GameLoop::InitStyles()
{
	
}

void GameLoop::AppPreInit()
{
	app = PlatformApplication::Get();
}

void GameLoop::AppInit()
{
	app->Initialize();

	String windowTitle = gProjectName;
	ProjectSettings* projectSettings = GetSettings<ProjectSettings>();
	if (projectSettings != nullptr)
	{
		windowTitle += " - v" + projectSettings->projectVersion;
	}

	mainWindow = app->InitMainWindow(windowTitle, gDefaultWindowWidth, gDefaultWindowHeight, false, false);
}

void GameLoop::AppPreShutdown()
{
	app->PreShutdown();
}

void GameLoop::AppShutdown()
{
	app->Shutdown();

	delete app;
	app = nullptr;
}


