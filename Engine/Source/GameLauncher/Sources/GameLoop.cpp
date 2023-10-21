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
	rtLayout.depthStencilFormat = RHI::DepthStencilFormat::None;

	viewport = RHI::gDynamicRHI->CreateViewport(mainWindow, width, height, false, rtLayout);

	cmdList = RHI::gDynamicRHI->CreateGraphicsCommandList(viewport);

	gEngine->Initialize();
}

void GameLoop::RunLoop()
{

}

void GameLoop::PreShutdown()
{

}

void GameLoop::Shutdown()
{

}

void GameLoop::LoadProject()
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

	mainWindow = app->InitMainWindow(gProjectName, gDefaultWindowWidth, gDefaultWindowHeight, false, false);
}

void GameLoop::AppPreShutdown()
{

}

void GameLoop::AppShutdown()
{

}

