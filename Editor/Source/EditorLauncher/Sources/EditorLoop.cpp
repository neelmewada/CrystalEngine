#include "EditorLauncher.h"

EditorLoop gEditorLoop{};

extern const CE::String css;

//rgb(21, 21, 21)

void EditorLoop::PreInit(int argc, char** argv)
{
	// Setup before loading any module

	// Set Core Globals before loading Core
	gProjectName = "CrystalEngine";

	gDefaultWindowWidth = 1280;
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

	cxxopts::Options options{ "Crystal Editor", "Crystal Editor command line arguments." };

	options.add_options()
		("h,help", "Print this help info and exit")
		("p,project", "Path of project file to open in editor", cxxopts::value<std::string>()->default_value(""))
		;

	try
	{
		cxxopts::ParseResult result = options.parse(argc, argv);

		if (result["h"].as<bool>())
		{
			std::cout << options.help() << std::endl;
			exit(0);
			return;
		}

		projectPath = result["p"].as<std::string>();

		if (!projectPath.Exists() || projectPath.GetExtension() != ProjectManager::Get().GetProjectFileExtension())
		{
			PlatformProcess::LaunchProcess(PlatformDirectories::GetAppRootDir() / "ProjectBrowser", "");
			exit(0);
			return;
		}

		gProjectPath = projectPath.GetParentPath();
		gProjectName = projectPath.GetFilename().RemoveExtension().GetString();
	}
	catch (std::exception exc)
	{
		CE_LOG(Error, All, "Failed to parse arguments: {}", exc.what());
		exit(-1);
	}
}

void EditorLoop::LoadStartupCoreModules()
{
	// Load Startup Modules
	ModuleManager::Get().LoadModule("Core");
	ModuleManager::Get().LoadModule("CoreApplication");

	// Load Rendering modules
	ModuleManager::Get().LoadModule("CoreRHI");
#if PAL_TRAIT_VULKAN_SUPPORTED
	ModuleManager::Get().LoadModule("VulkanRHI");
#else
#	error Vulkan API is required but not supported
#endif
}

void EditorLoop::LoadCoreModules()
{
	// Load other Core modules
	ModuleManager::Get().LoadModule("CoreMedia");

	ModuleManager::Get().LoadModule("CoreGUI");
	ModuleManager::Get().LoadModule("CoreWidgets");
}

void EditorLoop::LoadEngineModules()
{
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

	// Load non-important modules
	LoadCoreModules();

	RHI::gDynamicRHI->Initialize();

	AppInit();

	RHI::gDynamicRHI->PostInitialize();

	// Load engine modules
	LoadEngineModules();

	// Load editor modules
	LoadEditorModules();

	gEngine->PreInit();

	// Load RHI & GUI

	auto mainWindow = PlatformApplication::Get()->GetMainWindow();
	u32 width = 0, height = 0;
	mainWindow->GetDrawableWindowSize(&width, &height);
	mainWindow->GetUnderlyingHandle();

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

	unsigned char* defaultFont = nullptr;
	unsigned int defaultFontByteSize = 0;
	EditorStyles::Get().GetDefaultFont(&defaultFont, &defaultFontByteSize);

	Array<RHI::FontDesc> fontList{};

	fontList.Add({ defaultFontByteSize, 16, "Open Sans", false, defaultFont }); // Default Font & Size

	fontList.AddRange({
		{ defaultFontByteSize, 12, "Open Sans", false, defaultFont },
		{ defaultFontByteSize, 13, "Open Sans", false, defaultFont },
		{ defaultFontByteSize, 14, "Open Sans", false, defaultFont },
		{ defaultFontByteSize, 15, "Open Sans", false, defaultFont },
		{ defaultFontByteSize, 17, "Open Sans", false, defaultFont },
		{ defaultFontByteSize, 18, "Open Sans", false, defaultFont },
		{ defaultFontByteSize, 19, "Open Sans", false, defaultFont },
		{ defaultFontByteSize, 20, "Open Sans", false, defaultFont },
		{ defaultFontByteSize, 21, "Open Sans", false, defaultFont },
		{ defaultFontByteSize, 22, "Open Sans", false, defaultFont },
		{ defaultFontByteSize, 24, "Open Sans", false, defaultFont },
		{ defaultFontByteSize, 28, "Open Sans", false, defaultFont },
		{ defaultFontByteSize, 32, "Open Sans", false, defaultFont },
	});

	RHI::FontPreloadConfig fontConfig{};
	fontConfig.preloadFontCount = fontList.GetSize();
	fontConfig.preloadFonts = fontList.GetData();

	Array<void*> fontHandles{};

	cmdList->InitImGui(&fontConfig, fontHandles);

	CFontManager::Get().Initialize(fontList, fontHandles);
	EditorStyles::Get().InitDefaultStyle();

	InitStyles();

	gEngine->Initialize();

	editorWindow = CreateWidget<CrystalEditorWindow>(nullptr, "EditorWindow");
	editorWindow->SetAsDockSpaceWindow(true);
	editorWindow->SetTitle("Crystal Editor");
	editorWindow->SetFullscreen(true);
}

void EditorLoop::InitStyles()
{
	
}


void EditorLoop::RunLoop()
{
	using namespace CE::Widgets;

	while (!IsEngineRequestingExit())
	{
		app->Tick();
		// Engine
		gEngine->Tick();

		// Render
		viewport->SetClearColor(Color::Black());

		cmdList->Begin();
		cmdList->ImGuiNewFrame();

		editorWindow->Render();

		cmdList->ImGuiRender();
		cmdList->End();

		cmdList->ImGuiPlatformUpdate();

		if (RHI::gDynamicRHI->ExecuteCommandList(cmdList))
		{
			RHI::gDynamicRHI->PresentViewport(cmdList);
		}
	}
}

void EditorLoop::PreShutdown()
{
	editorWindow->RequestDestroy();
	editorWindow = nullptr;

	gEngine->PreShutdown();

	GetStyleManager()->PreShutdown();
	cmdList->ShutdownImGui();

	gEngine->Shutdown();

	// Load editor modules
	UnloadEditorModules();

	// Unload engine modules
	UnloadEngineModules();

	RHI::gDynamicRHI->DestroyCommandList(cmdList);
	RHI::gDynamicRHI->DestroyViewport(viewport);

	RHI::gDynamicRHI->PreShutdown();

	AppPreShutdown();

	// Shutdown core widgets before RHI device
	ModuleManager::Get().UnloadModule("CoreWidgets");

	RHI::gDynamicRHI->Shutdown();

	// Unload modules

	ModuleManager::Get().UnloadModule("CoreGUI");

	ModuleManager::Get().UnloadModule("CoreMedia");

#if PAL_TRAIT_VULKAN_SUPPORTED
	ModuleManager::Get().UnloadModule("VulkanRHI");
#endif
	ModuleManager::Get().UnloadModule("CoreRHI");
}

void EditorLoop::Shutdown()
{
	// Unload project
	UnloadProject();

	// Shutdown application
	AppShutdown();

	// Unload most important modules at last
	ModuleManager::Get().UnloadModule("CoreApplication");
	ModuleManager::Get().UnloadModule("Core");

	Logger::Shutdown();
}

void EditorLoop::LoadProject()
{
	if (!ProjectManager::Get().LoadProject(projectPath))
	{
		PlatformProcess::LaunchProcess(PlatformDirectories::GetAppRootDir() / "ProjectBrowser", "");
		RequestEngineExit("INVALID_PROJECT");
	}
}

void EditorLoop::UnloadProject()
{
	// Do nothing for now
}

void EditorLoop::AppPreInit()
{
	app = PlatformApplication::Get();
}

void EditorLoop::AppInit()
{
	app->Initialize();

	app->InitMainWindow("Crystal Editor", gDefaultWindowWidth, gDefaultWindowHeight, false, false);
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
