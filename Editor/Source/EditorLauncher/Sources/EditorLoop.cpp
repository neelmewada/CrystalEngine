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

		for (const auto& arg : positionalArgs)
		{
			IO::Path path = IO::Path(arg);

			if (path.Exists() && path.GetExtension() == ProjectManager::GetProjectFileExtension())
			{
				projectPath = path;
				break;
			}
		}

		if (projectPath.IsEmpty())
		{
			// Open project browser
			gProjectPath = PlatformDirectories::GetLaunchDir();
		}
		else
		{
			// TODO: Open editor
			gProjectPath = projectPath.GetParentPath();
			gProjectName = projectPath.GetFilename().RemoveExtension().GetString();
		}
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
	ModuleManager::Get().LoadModule("CoreShader");

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

	Prefs::Get().LoadPrefsJson();
}

void EditorLoop::UnloadEditorModules()
{
	Prefs::Get().SavePrefsJson();

	ModuleManager::Get().UnloadModule("CrystalEditor");

	ModuleManager::Get().UnloadModule("EditorSystem");

	ModuleManager::Get().UnloadModule("EditorWidgets");
	ModuleManager::Get().UnloadModule("EditorCore");
}


void EditorLoop::Init()
{
	// Load most important core modules for startup
	LoadStartupCoreModules();

	// Load application
	AppPreInit();
}

void EditorLoop::PostInit()
{
	using namespace CE::Widgets;

	LoadCoreModules();
	LoadEngineModules();
	LoadEditorModules();

	// Load Project
	LoadProject();

	AppInit();

	RHI::gDynamicRHI = new Vulkan::VulkanRHI();

	RHI::gDynamicRHI->Initialize();
	RHI::gDynamicRHI->PostInitialize();
  
	RPISystem::Get().Initialize();

	gEngine->Initialize();

	gEngine->PostInitialize();

	AssetManager* assetManager = AssetManager::Get();

	CE::Shader* standardShader = assetManager->LoadAssetAtPath<CE::Shader>("/Engine/Assets/Shaders/PBR/Standard");
	CE::Shader* iblConvolutionShader = assetManager->LoadAssetAtPath<CE::Shader>("/Engine/Assets/Shaders/CubeMap/IBLConvolution");
	CE::Shader* textureGenShader = assetManager->LoadAssetAtPath<CE::Shader>("/Engine/Assets/Shaders/Utils/TextureGen");

	RPI::RPISystemInitInfo rpiInitInfo{};
	rpiInitInfo.standardShader = standardShader->GetShaderCollection();
	rpiInitInfo.iblConvolutionShader = iblConvolutionShader->GetShaderCollection();
	rpiInitInfo.textureGenShader = textureGenShader->GetShaderCollection();

	RPISystem::Get().PostInitialize(rpiInitInfo);

	CApplication::Get()->LoadGlobalStyleSheet(PlatformDirectories::GetLaunchDir() / "Editor/Styles/EditorStyle.css");

	auto tickDelegate = MemberDelegate(&EditorLoop::AlternateTick, this);
	this->tickDelegateHandle = tickDelegate.GetHandle();
	app->AddTickHandler(tickDelegate);

	if (!projectPath.Exists())
	{
		ProjectBrowser* projectBrowser = CreateWindow<ProjectBrowser>("ProjectBrowser", mainWindow);
		projectBrowser->Show();
	}
	else
	{
		CrystalEditorWindow* editorWindow = CreateWindow<CrystalEditorWindow>("CrystalEditor", mainWindow);
		editorWindow->SetTitle("Crystal Editor");
		editorWindow->Show();
	}
}

void EditorLoop::InitStyles()
{

}

void EditorLoop::AlternateTick()
{
	auto app = PlatformApplication::Get();

	auto curTime = clock();
	deltaTime = (f32)(curTime - previousTime) / CLOCKS_PER_SEC;

	gEngine->Tick(deltaTime);

	previousTime = curTime;
}

void EditorLoop::RunLoop()
{
	using namespace CE::Widgets;

	while (!IsEngineRequestingExit())
	{
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

void EditorLoop::PreShutdown()
{
	auto app = PlatformApplication::Get();
	app->RemoveTickHandler(tickDelegateHandle);

	// Destroy the project manager, which consequently saves it's Prefs.
	auto projectManager = ProjectManager::TryGet();
	if (projectManager)
	{
		projectManager->Destroy();
	}

	// Save project & settings, and unload
	if (projectPath.Exists())
	{
		SaveSettings();
		UnloadSettings();
	}

	gEngine->PreShutdown();
  
	RPISystem::Get().Shutdown();

	gDynamicRHI->PreShutdown();

	InputManager::Get().Shutdown(app);
	app->PreShutdown();

	gEngine->Shutdown();

	// Load editor modules
	UnloadEditorModules();

	// Unload engine modules
	UnloadEngineModules();

	AppPreShutdown();

	// Shutdown core widgets before RHI device
	ModuleManager::Get().UnloadModule("CrystalWidgets");

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

void EditorLoop::Shutdown()
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

void EditorLoop::LoadProject()
{
	if (!projectPath.IsEmpty())
	{
		bool success = ProjectManager::Get()->LoadProject(projectPath);
		if (!success)
		{
			PlatformProcess::LaunchProcess(PlatformDirectories::GetLaunchDir() / "EditorLauncher", "");
			exit(0);
		}
	}
}

void EditorLoop::AppPreInit()
{
	app = PlatformApplication::Get();
	app->Initialize();

	InputManager::Get().Initialize(app);
	
}

void EditorLoop::AppInit()
{
	gEngine->PreInit();
	bool isProjectBrowsingMode = false;

	gDefaultWindowWidth = 1366;
	gDefaultWindowHeight = 768;

	Vec2i screenSize = app->GetMainScreenSize();
	if (screenSize.x >= 2560 && screenSize.y >= 1440)
	{
		gDefaultWindowWidth = 1920;
		gDefaultWindowHeight = 1080;
	}

	PlatformWindowInfo windowInfo{};
	windowInfo.maximised = windowInfo.fullscreen = windowInfo.hidden = false;
	windowInfo.resizable = true;
	windowInfo.windowFlags = PlatformWindowFlags::DestroyOnClose;
	if (projectPath.IsEmpty())
	{
		isProjectBrowsingMode = true;
		windowInfo.windowFlags |= PlatformWindowFlags::Utility;
		gDefaultWindowWidth = 1024;
		gDefaultWindowHeight = 640;
	}

	mainWindow = app->InitMainWindow(MODULE_NAME, gDefaultWindowWidth, gDefaultWindowHeight, windowInfo);

	mainWindow->SetMinimumSize(isProjectBrowsingMode ? Vec2i(gDefaultWindowWidth, gDefaultWindowHeight) : Vec2i(1280, 720));
	mainWindow->SetBorderless(true);
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
