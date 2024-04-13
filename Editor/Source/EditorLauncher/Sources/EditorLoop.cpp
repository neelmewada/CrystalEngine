#include "EditorLauncher.h"

EditorLoop gEditorLoop{};

extern const CE::String css;

void EditorLoop::PreInit(int argc, char** argv)
{
	// Setup before loading any module

	// Set Core Globals before loading Core
	gProjectName = "CrystalEngine";

	gDefaultWindowWidth = 1280;
	gDefaultWindowHeight = 720;

	gProgramArguments.Clear();
	for (int i = 0; i < argc; i++)
	{
		gProgramArguments.Add(argv[i]);
	}

	cxxopts::Options options{ "Crystal Editor", "Crystal Editor command line arguments." };

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
		}

		auto positionalArgs = result.unmatched();
		bool foundProject = false;

		for (const auto& arg : positionalArgs)
		{
			IO::Path path = IO::Path(arg);

			if (path.Exists() && path.GetExtension() == ProjectManager::Get().GetProjectFileExtension())
			{
				foundProject = true;
				projectPath = path;
				break;
			}
		}

		gProjectPath = projectPath.GetParentPath();
		gProjectName = projectPath.GetFilename().RemoveExtension().GetString();
	}
	catch (const std::exception& exc)
	{
		CE_LOG(Error, All, "Failed to parse arguments: {}", exc.what());
		exit(-1);
	}

	// Initialize logging
	Logger::Initialize();
	Logger::SetConsoleLogLevel(LogLevel::Trace);
	Logger::SetFileDumpLogLevel(LogLevel::Trace);
}

void EditorLoop::LoadStartupCoreModules()
{
	// Load Startup Modules
	ModuleManager::Get().LoadModule("Core");
	ModuleManager::Get().LoadModule("CoreApplication");
	ModuleManager::Get().LoadModule("CoreInput");

	// Settings module
	ModuleManager::Get().LoadModule("CoreSettings");

	// Load Rendering modules
	ModuleManager::Get().LoadModule("CoreRHI");
#if PAL_TRAIT_VULKAN_SUPPORTED
	ModuleManager::Get().LoadModule("VulkanRHI");
#else
#	error Vulkan API is required
#endif
}

void EditorLoop::LoadCoreModules()
{
	// Load other Core modules
	ModuleManager::Get().LoadModule("CoreMedia");
	ModuleManager::Get().LoadModule("CoreMesh");

	ModuleManager::Get().LoadModule("CrystalWidgets");
}

void EditorLoop::LoadEngineModules()
{
	ModuleManager::Get().LoadModule("CoreRPI");
	ModuleManager::Get().LoadModule("System");
}

void EditorLoop::UnloadEngineModules()
{
	ModuleManager::Get().UnloadModule("System");
}

void EditorLoop::LoadEditorModules()
{
	ModuleManager::Get().LoadModule("EditorCore");
	ModuleManager::Get().LoadModule("EditorWidgets");

	ModuleManager::Get().LoadModule("EditorSystem");

	ModuleManager::Get().LoadModule("CrystalEditor");
}

void EditorLoop::UnloadEditorModules()
{
	ModuleManager::Get().UnloadModule("CrystalEditor");

	ModuleManager::Get().UnloadModule("EditorSystem");

	ModuleManager::Get().UnloadModule("EditorWidgets");
	ModuleManager::Get().UnloadModule("EditorCore");
}


void EditorLoop::Init()
{
	// Load most important core modules for startup
	LoadStartupCoreModules();

	// Load Project
	LoadProject();

	// Load application
	AppPreInit();
}

void EditorLoop::PostInit()
{
	using namespace CE::Widgets;

	LoadCoreModules();
	LoadEngineModules();

	AppInit();
}

void EditorLoop::InitStyles()
{
	
}


void EditorLoop::RunLoop()
{
	using namespace CE::Widgets;

	while (!IsEngineRequestingExit())
	{
		auto curTime = clock();
		f32 deltaTime = ((f32)(curTime - previousTime)) / CLOCKS_PER_SEC;

		app->Tick();
		// Engine
		gEngine->Tick(deltaTime);
		
		

		previousTime = curTime;
	}
}

void EditorLoop::PreShutdown()
{
	

	// Save project & settings, and unload
	SaveSettings();
	UnloadSettings();

	gEngine->PreShutdown();

	gEngine->Shutdown();

	// Load editor modules
	UnloadEditorModules();

	// Unload engine modules
	UnloadEngineModules();

	RHI::gDynamicRHI->PreShutdown();

	AppPreShutdown();

	// Shutdown core widgets before RHI device
	ModuleManager::Get().UnloadModule("CrystalWidgets");

	RHI::gDynamicRHI->Shutdown();

	// Unload modules

	ModuleManager::Get().UnloadModule("CoreMedia");
	ModuleManager::Get().UnloadModule("CoreMesh");

#if PAL_TRAIT_VULKAN_SUPPORTED
	ModuleManager::Get().UnloadModule("VulkanRHI");
#endif
	ModuleManager::Get().UnloadModule("CoreRHI");
}

void EditorLoop::Shutdown()
{
	// Shutdown application
	AppShutdown();

	// Unload settings module
	ModuleManager::Get().UnloadModule("CoreSettings");

	// Unload most important modules at last
	ModuleManager::Get().UnloadModule("CoreInput");
	ModuleManager::Get().UnloadModule("CoreApplication");
	ModuleManager::Get().UnloadModule("Core");

	Logger::Shutdown();
}

void EditorLoop::LoadProject()
{
	gProjectPath = PlatformDirectories::GetLaunchDir();

	if (!projectPath.IsEmpty())
	{
		
	}
}

void EditorLoop::AppPreInit()
{
	app = PlatformApplication::Get();
	app->Initialize();
	InputManager::Get().Initialize(app);

	gEngine->PreInit();

	gDefaultWindowWidth = 1280;
	gDefaultWindowHeight = 720;

	PlatformWindowInfo windowInfo{};
	windowInfo.maximised = windowInfo.fullscreen = windowInfo.hidden = false;
	windowInfo.resizable = true;
	windowInfo.windowFlags = PlatformWindowFlags::DestroyOnClose;

	PlatformWindow* mainWindow = app->InitMainWindow(MODULE_NAME, gDefaultWindowWidth, gDefaultWindowHeight, false, false);
	mainWindow->SetBorderless(true);

	
}

void EditorLoop::AppInit()
{
	
}

void EditorLoop::AppPreShutdown()
{
	app->PreShutdown();
}

void EditorLoop::AppShutdown()
{
	app->Shutdown();

	delete app;
	app = nullptr;
}
