#include "GameLauncher.h"

GameLoop gGameLoop{};

void GameLoop::PreInit(int argc, char** argv)
{
	// Setup before loading anything

	gProjectPath = PlatformDirectories::GetLaunchDir();
	String folderName = gProjectPath.GetFileName().GetString();

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

	ModuleManager::Get().LoadModule("CoreSettings");
	ModuleManager::Get().LoadModule("CoreShader");

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
	ModuleManager::Get().LoadModule("CoreMesh");
}

void GameLoop::LoadEngineModules()
{
	ModuleManager::Get().LoadModule("CoreRPI");
	ModuleManager::Get().LoadModule("FusionCore");
	ModuleManager::Get().LoadModule("Fusion");

	ModuleManager::Get().LoadModule("System");
	ModuleManager::Get().LoadModule("GameSystem");
}

void GameLoop::UnloadEngineModules()
{
	ModuleManager::Get().UnloadModule("Fusion");
	ModuleManager::Get().UnloadModule("FusionCore");

	ModuleManager::Get().UnloadModule("GameSystem");
	ModuleManager::Get().UnloadModule("System");

	// CoreRPI should always shutdown after the engine modules!
	ModuleManager::Get().UnloadModule("CoreRPI");
}

void GameLoop::Init()
{
	// Load most important core modules for startup
	LoadStartupCoreModules();

	// Load application
	AppPreInit();
}

void GameLoop::PostInit()
{
	LoadCoreModules();
	LoadEngineModules();

	// Load Project
	LoadProject();

	AppInit();

	RHI::gDynamicRHI = new Vulkan::VulkanRHI();

	RHI::gDynamicRHI->Initialize();
	RHI::gDynamicRHI->PostInitialize();

	RHI::FrameSchedulerDescriptor desc{};
	desc.numFramesInFlight = 2;
	RHI::FrameScheduler::Create(desc);

	RPI::RPISystem::Get().Initialize();

	FusionApplication* fApp = FusionApplication::Get();

	// Initialize engine & it's subsystems, and set game viewport
	gEngine->Initialize();

	// Post initialize
	gEngine->PostInitialize();

	AssetManager* assetManager = AssetManager::Get();

	CE::Shader* standardShader = assetManager->LoadAssetAtPath<CE::Shader>("/Engine/Assets/Shaders/PBR/Standard");
	CE::Shader* iblConvolutionShader = assetManager->LoadAssetAtPath<CE::Shader>("/Engine/Assets/Shaders/CubeMap/IBLConvolution");
	CE::Shader* textureGenShader = assetManager->LoadAssetAtPath<CE::Shader>("/Engine/Assets/Shaders/Utils/TextureGen");

	RPI::RPISystemInitInfo rpiInitInfo{};
	rpiInitInfo.standardShader = standardShader->GetShaderCollection();
	rpiInitInfo.iblConvolutionShader = iblConvolutionShader->GetShaderCollection();
	rpiInitInfo.textureGenShader = textureGenShader->GetShaderCollection();

	RPI::RPISystem::Get().PostInitialize(rpiInitInfo);

	auto tickDelegate = MemberDelegate(&GameLoop::ExposedTick, this);
	this->tickDelegateHandle = tickDelegate.GetHandle();
	app->AddTickHandler(tickDelegate);

	SetupTestScene();
}

void GameLoop::ExposedTick()
{
	auto app = PlatformApplication::Get();

	auto curTime = clock();
	deltaTime = (f32)(curTime - previousTime) / CLOCKS_PER_SEC;

	FusionApplication::Get()->SetExposed();

	gEngine->Tick(deltaTime);

	previousTime = curTime;
}

void GameLoop::RunLoop()
{
	while (!IsEngineRequestingExit())
	{
		FusionApplication::Get()->ResetExposed();

		auto curTime = clock();
		deltaTime = (f32)(curTime - previousTime) / CLOCKS_PER_SEC;

		// App & Input Tick
		app->Tick();
		InputManager::Get().Tick();

		// Engine tick
		gEngine->Tick(deltaTime);

		previousTime = curTime;
	}
}

void GameLoop::PreShutdown()
{
	auto app = PlatformApplication::Get();
	app->RemoveTickHandler(tickDelegateHandle);

	// Save project & settings, and unload
	if (projectPath.Exists())
	{
		SaveSettings();
		UnloadSettings();
	}

	FusionApplication* fApp = FusionApplication::Get();

	fApp->PreShutdown();

	fApp->Shutdown();
	fApp->Destroy();

	gEngine->PreShutdown();

	// fApp is Shutdown and destroyed by RendererSubsystem, no need to do it again here.

	RPI::RPISystem::Get().Shutdown();

	RHI::gDynamicRHI->PreShutdown();

	InputManager::Get().Shutdown(app);
	app->PreShutdown();

	gEngine->Shutdown();

	// Unload engine modules
	UnloadEngineModules();

	AppPreShutdown();

	// Shutdown core widgets before RHI device
	ModuleManager::Get().UnloadModule("Fusion");
	ModuleManager::Get().UnloadModule("FusionCore");

	RHI::gDynamicRHI->Shutdown();

	// Unload modules

	ModuleManager::Get().UnloadModule("CoreShader");
	ModuleManager::Get().UnloadModule("CoreMedia");
	ModuleManager::Get().UnloadModule("CoreMesh");

#if PAL_TRAIT_VULKAN_SUPPORTED
	ModuleManager::Get().UnloadModule("VulkanRHI");
#endif
	ModuleManager::Get().UnloadModule("CoreRHI");
}

void GameLoop::Shutdown()
{
	IO::Path exitLaunchProcess = gExitLaunchProcess;
	String exitLaunchArgs = gExitLaunchArguments;

	// Shutdown application
	AppShutdown();

	// Unload settings module
	ModuleManager::Get().UnloadModule("CoreSettings");

	// Unload most important modules at last
	ModuleManager::Get().UnloadModule("CoreInput");
	ModuleManager::Get().UnloadModule("CoreApplication");
	ModuleManager::Get().UnloadModule("Core");

	Logger::Shutdown();

	if (!exitLaunchProcess.IsEmpty())
	{
		PlatformProcess::LaunchProcess(exitLaunchProcess, exitLaunchArgs);
	}
}

void GameLoop::SetupTestScene()
{
	AssetManager* assetManager = AssetManager::Get();

	TextureCube* skybox = assetManager->LoadAssetAtPath<TextureCube>("/Engine/Assets/Textures/HDRI/sample_night");
	CE::Shader* standardShader = assetManager->LoadAssetAtPath<CE::Shader>("/Engine/Assets/Shaders/PBR/Standard");
	CE::Shader* skyboxShader = assetManager->LoadAssetAtPath<CE::Shader>("/Engine/Assets/Shaders/PBR/SkyboxCubeMap");

	CE::Scene* scene = CreateObject<CE::Scene>(gEngine, "TestScene");

	GameViewportSubsystem* viewportSubsystem = gEngine->GetSubsystem<GameViewportSubsystem>();
	gameWindow = viewportSubsystem->GetGameWindow();

	gameWindow->SetScene(scene->GetRpiScene());
	scene->SetSkyboxCubeMap(skybox);

	{
		// - Textures & Materials -

		CE::Texture* albedoTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/Aluminum/albedo");
		CE::Texture* normalTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/Aluminum/normal");
		CE::Texture* metallicTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/Aluminum/metallic");
		CE::Texture* roughnessTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/Aluminum/roughness");

		CE::Texture* plasticAlbedoTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/Plastic/albedo");
		CE::Texture* plasticNormalTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/Plastic/normal");
		CE::Texture* plasticMetallicTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/Plastic/metallic");
		CE::Texture* plasticRoughnessTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Textures/Plastic/roughness");

		CE::Material* material = CreateObject<CE::Material>(scene, "Material");
		material->SetShader(standardShader);

		material->SetProperty("_AlbedoTex", albedoTex);
		material->SetProperty("_NormalTex", normalTex);
		material->SetProperty("_MetallicTex", metallicTex);
		material->SetProperty("_RoughnessTex", roughnessTex);
		material->ApplyProperties();

		CE::Material* plasticMaterial = CreateObject<CE::Material>(scene, "PlasticMaterial");
		plasticMaterial->SetShader(standardShader);

		plasticMaterial->SetProperty("_AlbedoTex", plasticAlbedoTex);
		plasticMaterial->SetProperty("_NormalTex", plasticNormalTex);
		plasticMaterial->SetProperty("_MetallicTex", plasticMetallicTex);
		plasticMaterial->SetProperty("_RoughnessTex", plasticRoughnessTex);
		plasticMaterial->ApplyProperties();

		StaticMesh* sphereMesh = CreateObject<StaticMesh>(scene, "SphereMesh");
		{
			RPI::ModelAsset* sphereModel = CreateObject<RPI::ModelAsset>(sphereMesh, "SphereModel");
			RPI::ModelLodAsset* sphereLodAsset = RPI::ModelLodAsset::CreateSphereAsset(sphereModel);
			sphereModel->AddModelLod(sphereLodAsset);

			sphereMesh->SetModelAsset(sphereModel);
		}

		// - Camera -
		
		CameraActor* camera = CreateObject<CameraActor>(scene, "Camera");
		camera->GetCameraComponent()->SetLocalPosition(Vec3(0, 0, 0));
		scene->AddActor(camera);

		CameraComponent* cameraComponent = camera->GetCameraComponent();
		cameraComponent->SetFieldOfView(60);

		// - Skybox -

		StaticMeshActor* skyboxActor = CreateObject<StaticMeshActor>(scene, "SkyboxActor");
		scene->AddActor(skyboxActor);

		StaticMeshComponent* skyboxMeshComponent = skyboxActor->GetMeshComponent();
		skyboxMeshComponent->SetStaticMesh(sphereMesh);

		skyboxMeshComponent->SetLocalPosition(Vec3(0, 0, 0));
		skyboxMeshComponent->SetLocalScale(Vec3(1, 1, 1) * 1000);

		{
			CE::Material* skyboxMaterial = CreateObject<CE::Material>(skyboxMeshComponent, "Material");
			skyboxMaterial->SetShader(skyboxShader);
			skyboxMeshComponent->SetMaterial(skyboxMaterial, 0, 0);

			skyboxMaterial->SetProperty("_CubeMap", skybox);
			skyboxMaterial->ApplyProperties();
		}
	}

	gEngine->AddRenderViewport(gameWindow);
	gEngine->LoadScene(scene);
}

void GameLoop::LoadProject()
{
	// Load project (aka Setings)
	Bundle* settingsBundle = GetSettingsBundle();

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
	gEngine->PreInit();
	app->Initialize();

	InputManager::Get().Initialize(app);

	String windowTitle = gProjectName;
	ProjectSettings* projectSettings = GetSettings<ProjectSettings>();
	if (projectSettings != nullptr)
	{
		windowTitle += " - v" + projectSettings->projectVersion;
	}

	mainWindow = app->InitMainWindow(windowTitle, gDefaultWindowWidth, gDefaultWindowHeight, false, false);
	mainWindow->Show();
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


