#include "EditorLauncher.h"

EditorLoop gEditorLoop{};

extern const CE::String css;

//rgb(21, 21, 21)

void EditorLoop::PreInit(int argc, char** argv)
{
	// Setup before loading any module

	// Set Core Globals before loading Core
	gProjectName = "CrystalEngine";

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
			return;
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

		if (!foundProject)
		{
			// Open the project browser as an external process and exit
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

	// Settings module
	ModuleManager::Get().LoadModule("CoreSettings");

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

	// Load non-important core modules
	LoadCoreModules();

	RHI::gDynamicRHI->Initialize();

	AppInit();

	RHI::gDynamicRHI->PostInitialize();

	// Load engine modules
	LoadEngineModules();

	// Load editor modules
	LoadEditorModules();

	gEngine->PreInit();

	// Load RHI::Viewport and initialize GUI

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
		{ defaultFontByteSize, 26, "Open Sans", false, defaultFont },
		{ defaultFontByteSize, 28, "Open Sans", false, defaultFont },
		{ defaultFontByteSize, 30, "Open Sans", false, defaultFont },
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
		auto curTime = clock();
		f32 deltaTime = ((f32)(curTime - previousTime)) / CLOCKS_PER_SEC;

		app->Tick();
		// Engine
		gEngine->Tick(deltaTime);
		
		// Render
		viewport->SetClearColor(Color::Black());

		cmdList->Begin();
		cmdList->ImGuiNewFrame();

		editorWindow->Render();

		static bool showDemo = false;
		if (showDemo)
			GUI::ShowDemoWindow(&showDemo);

		cmdList->ImGuiRender();
		cmdList->End();

		cmdList->ImGuiPlatformUpdate();

		if (RHI::gDynamicRHI->ExecuteCommandList(cmdList))
		{
			RHI::gDynamicRHI->PresentViewport(cmdList);
		}

		previousTime = curTime;
	}
}

void EditorLoop::PreShutdown()
{
	editorWindow->Destroy();
	editorWindow = nullptr;

	// Save project & settings, and unload
	SaveSettings();
	UnloadSettings();

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
	// Shutdown application
	AppShutdown();

	// Unload settings module
	ModuleManager::Get().UnloadModule("CoreSettings");

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
