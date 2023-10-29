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

	while (!IsEngineRequestingExit())
	{
		auto curTime = clock();
		f32 deltaTime = ((f32)(curTime - previousTime)) / CLOCKS_PER_SEC;

		app->Tick();

		// - Engine -
		gEngine->Tick(deltaTime);

		if (!sceneSubsystem)
			sceneSubsystem = gEngine->GetSubsystem<SceneSubsystem>();

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

		cmdList->End();

		if (RHI::gDynamicRHI->ExecuteCommandList(cmdList))
		{
			RHI::gDynamicRHI->PresentViewport(cmdList);
		}
		
		previousTime = curTime;
	}
}

void GameLoop::PreShutdown()
{
	// Unload game code

	UnloadSettings();

	gEngine->PreShutdown();

	gEngine->GetPrimaryGameViewport()->Shutdown();

	gEngine->Shutdown();

	ModuleManager::Get().UnloadModule("Sandbox");

	UnloadEngineModules();

	RHI::gDynamicRHI->DestroyCommandList(cmdList);
	RHI::gDynamicRHI->DestroyViewport(viewport);

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


