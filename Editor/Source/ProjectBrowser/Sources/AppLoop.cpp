#include "Main.h"

AppLoop gLoop{};

void AppLoop::PreInit(int argc, const char** argv)
{
	// Setup before loading any module

	// Set Core Globals before loading Core
	gProjectName = "Project Browser";
	gProjectPath = PlatformDirectories::GetLaunchDir();

	gDefaultWindowWidth = 854;
	gDefaultWindowHeight = 480;

	// Initialize logging
	Logger::Initialize();

	gProgramArguments.Clear();
	for (int i = 0; i < argc; i++)
	{
		gProgramArguments.Add(argv[i]);
	}

	cxxopts::Options options{ "Project Browser", "A GUI application to create and open projects in Crystal Editor." };

	options.add_options()
		("h,help", "Print this help info")
	;

	try
	{
		cxxopts::ParseResult result = options.parse(argc, argv);

		if (result["h"].as<bool>())
		{
			std::cout << options.help() << std::endl;
			exit(0);
		}
	}
	catch (std::exception exc)
	{
		CE_LOG(Error, All, "Failed to parse arguments: {}", exc.what());
		exit(-1);
	}
}

void AppLoop::LoadStartupCoreModules()
{
	// Load Startup Modules
	ModuleManager::Get().LoadModule("Core");
	ModuleManager::Get().LoadModule("CoreApplication");

	// Load Rendering modules
	ModuleManager::Get().LoadModule("CoreRHI");
#if PAL_TRAIT_VULKAN_SUPPORTED
	ModuleManager::Get().LoadModule("VulkanRHI");
#endif

	ModuleManager::Get().LoadModule("CoreGUI");
	ModuleManager::Get().LoadModule("CoreWidgets");
}

void AppLoop::LoadCoreModules()
{
	// Load other Core modules
	ModuleManager::Get().LoadModule("CoreMedia");
}

void AppLoop::Init()
{
	// Load most important core modules for startup
	LoadStartupCoreModules();

	AppPreInit();
}

void AppLoop::PostInit()
{
	using namespace CE::Widgets;

	// Load non-important modules
	LoadCoreModules();

	RHI::gDynamicRHI->Initialize();

	AppInit();

	RHI::gDynamicRHI->PostInitialize();

	auto mainWindow = PlatformApplication::Get()->GetMainWindow();
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

	unsigned char* defaultFont = nullptr;
	unsigned int defaultFontByteSize = 0;
	EditorStyles::Get().GetDefaultFont(&defaultFont, &defaultFontByteSize);

	Array<RHI::FontDesc> fontList{};

	fontList.Add({ defaultFontByteSize, 16, "Open Sans", false, defaultFont }); // Default Font & Size

	fontList.AddRange({ 
		{ defaultFontByteSize, 14, "Open Sans", false, defaultFont },
		{ defaultFontByteSize, 15, "Open Sans", false, defaultFont },
		{ defaultFontByteSize, 17, "Open Sans", false, defaultFont },
		{ defaultFontByteSize, 18, "Open Sans", false, defaultFont },
		{ defaultFontByteSize, 20, "Open Sans", false, defaultFont },
		{ defaultFontByteSize, 24, "Open Sans", false, defaultFont },
		{ defaultFontByteSize, 28, "Open Sans", false, defaultFont },
	});

	RHI::FontPreloadConfig fontConfig{};
	fontConfig.preloadFontCount = fontList.GetSize();
	fontConfig.preloadFonts = fontList.GetData();

	Array<void*> fontHandles{};

	cmdList->InitImGui(&fontConfig, fontHandles);

	CFontManager::Get().Initialize(fontList, fontHandles);
	EditorStyles::Get().InitDefaultStyle();

	gStyleManager->PushGlobal();

	projectBrowser = CreateWidget<ProjectBrowserWindow>(nullptr, "ProjectBrowser");
}

void AppLoop::InitStyles()
{

}

void AppLoop::RunLoop()
{
	using namespace CE::Widgets;

	while (!IsEngineRequestingExit())
	{
		app->Tick();

		// Render
		viewport->SetClearColor(Color::FromRGBA32(26, 184, 107));

		cmdList->Begin();
		cmdList->ImGuiNewFrame();

		projectBrowser->RenderGUI();

		cmdList->ImGuiRender();
		cmdList->End();

		cmdList->ImGuiPlatformUpdate();

		if (RHI::gDynamicRHI->ExecuteCommandList(cmdList))
		{
			RHI::gDynamicRHI->PresentViewport(cmdList);
		}
	}
}

void AppLoop::PreShutdown()
{
	projectBrowser->RequestDestroy();
	projectBrowser = nullptr;

	gStyleManager->PopGlobal();

	cmdList->ShutdownImGui();

	RHI::gDynamicRHI->DestroyCommandList(cmdList);
	RHI::gDynamicRHI->DestroyViewport(viewport);

	RHI::gDynamicRHI->PreShutdown();

	AppPreShutdown();

	RHI::gDynamicRHI->Shutdown();

	// Unload modules

	ModuleManager::Get().UnloadModule("CoreWidgets");
	ModuleManager::Get().UnloadModule("CoreGUI");

	ModuleManager::Get().UnloadModule("CoreMedia");

#if PAL_TRAIT_VULKAN_SUPPORTED
	ModuleManager::Get().UnloadModule("VulkanRHI");
#endif
	ModuleManager::Get().UnloadModule("CoreRHI");
}

void AppLoop::Shutdown()
{
	AppShutdown();

	// Unload most important modules at last
	ModuleManager::Get().UnloadModule("CoreApplication");
	ModuleManager::Get().UnloadModule("Core");

	Logger::Shutdown();
}

void AppLoop::AppPreInit()
{
	app = PlatformApplication::Get();
}

void AppLoop::AppInit()
{
	app->Initialize();

	app->InitMainWindow("Project Browser", gDefaultWindowWidth, gDefaultWindowHeight, false, false, false);
}

void AppLoop::AppPreShutdown()
{
	app->PreShutdown();
}

void AppLoop::AppShutdown()
{
	app->Shutdown();

	delete app;
	app = nullptr;
}

