
#include "SandboxLoop.h"
#include "OpenSans.h"

using namespace CE;

void SandboxLoop::PreInit(int argc, const char** argv)
{
    // Setup before loading any module

    // Set Core Globals before loading Core
    gProjectName = "Sandbox";
    
    gProgramArguments.Clear();
    for (int i = 0; i < argc; i++)
    {
        gProgramArguments.Add(argv[i]);
    }

    // Initialize logging
    Logger::Initialize();
}

void SandboxLoop::LoadStartupCoreModules()
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

void SandboxLoop::LoadCoreModules()
{
    // Load other Core modules
    ModuleManager::Get().LoadModule("CoreMedia");
}

void SandboxLoop::Init()
{
    // Load most important core modules for startup
    LoadStartupCoreModules();

    AppPreInit();
}

void SandboxLoop::PostInit()
{
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

	RHI::FontDesc openSans{};
	openSans.fontName = "Open Sans";
	openSans.byteSize = OpenSans_VariableFont_len;
	openSans.fontData = OpenSans_VariableFont_ttf;
	openSans.pointSize = 16;

	RHI::FontPreloadConfig fontConfig{};
	fontConfig.preloadFontCount = 1;
	fontConfig.preloadFonts = &openSans;

    cmdList->InitImGui(&fontConfig);
    
    // Setup GUI
    using namespace CE::Widgets;
    
    window = CreateObject<CWindow>(nullptr, "TestWindow");
    window->SetWidgetFlags(WidgetFlags::None);
    
    SetupGUI();
}

void SandboxLoop::PreShutdown()
{
	gStyleManager->PopGlobal();

    window->RequestDestroy();
    window = nullptr;
    
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

void SandboxLoop::Shutdown()
{
    AppShutdown();

    // Unload most important modules at last
    ModuleManager::Get().UnloadModule("CoreApplication");
    ModuleManager::Get().UnloadModule("Core");

    Logger::Shutdown();
}

void SandboxLoop::AppPreInit()
{
    app = PlatformApplication::Get();
}

void SandboxLoop::AppInit()
{
    app->Initialize();

    app->InitMainWindow(gProjectName, gDefaultWindowWidth, gDefaultWindowHeight, false, false);
}

void SandboxLoop::AppPreShutdown()
{
    app->PreShutdown();
}

void SandboxLoop::AppShutdown()
{
    app->Shutdown();

    delete app;
    app = nullptr;
}

void SandboxLoop::SetupGUI()
{
    using namespace CE::Widgets;

    
    window->SetTitle("Test Window");
	//window->GetStyle().AddProperty(CStyleProperty::ForegroundColor, Color(0, 0, 1, 1));

	//CStackLayout* horizontal = CreateObject<CStackLayout>(window, "Horizontal");
	//horizontal->SetDirection(CStackDirection::Horizontal);
    
    CLabel* label = CreateObject<CLabel>(window, "MyLabel");
    label->SetText("Label Text");
    label->GetStyle().AddProperty(CStylePropertyType::ForegroundColor, Color::White());
	label->GetStyle().AddProperty(CStylePropertyType::Background, Color(0.5f, 0.5f, 0.0f, 1.0f));
	label->GetStyle().AddProperty(CStylePropertyType::Background, Color(0.0f, 1.0f, 0.0f, 1.0f), CStateFlag::Hovered);
	label->GetStyle().AddProperty(CStylePropertyType::Background, Color(0.0f, 0.0f, 1.0f, 1.0f), CStateFlag::Pressed);

	CLabel* label2 = CreateObject<CLabel>(window, "Label2");
	label2->SetText("This is a long form label widget! And this is another sentence in the same label.");
	
	/*CStackLayout* vertical = CreateObject<CStackLayout>(horizontal, "Vertical");
	vertical->SetDirection(CStackDirection::Vertical);

	CLabel* vertLabel = CreateObject<CLabel>(vertical, "VertLabel");
	vertLabel->SetText("Vert Label");

	CButton* vertButton = CreateObject<CButton>(vertical, "VertButton");
	vertButton->SetText("Vert Button");*/

	CButton* button = CreateObject<CButton>(window, "MyButton");
	button->SetText("Click Me!");
	button->GetStyle().AddProperty(CStylePropertyType::BorderRadius, Vec4(5, 5, 5, 5));
	button->GetStyle().AddProperty(CStylePropertyType::Background, Color(0.5f, 0.5f, 0.0f, 1.0f));
	button->GetStyle().AddProperty(CStylePropertyType::Background, Color(0.0f, 1.0f, 0.0f, 1.0f), CStateFlag::Hovered);
	button->GetStyle().AddProperty(CStylePropertyType::Background, Color(0.0f, 0.0f, 1.0f, 1.0f), CStateFlag::Pressed);
	button->GetStyle().AddProperty(CStylePropertyType::Width, CStyleValue(50, true));
	button->GetStyle().AddProperty(CStylePropertyType::MinHeight, 50);
	
	Object::Bind(button, MEMBER_FUNCTION(CButton, OnButtonClicked), []()
		{
			CE_LOG(Info, All, "Button Clicked from lambda!");
		});

	/*CLabel* newLineLabel = CreateObject<CLabel>(window, "NewLine");
	newLineLabel->SetText("New Line");*/
}

void SandboxLoop::RunLoop()
{
    while (!IsEngineRequestingExit())
    {
        app->Tick();

        // Render
        viewport->SetClearColor(Color::FromRGBA32(26, 184, 107));

        cmdList->Begin();
        cmdList->ImGuiNewFrame();

        GUI::BeginWindow("DockSpaceWindow", nullptr, GUI::WF_FullScreen | GUI::WF_MenuBar | GUI::WF_NoPadding);
        {
            auto dockSpaceId = GUI::DockSpace("MainDockSpace");

            if (GUI::BeginMenuBar())
            {
                if (GUI::BeginMenu("File"))
                {
                    GUI::MenuItem("New");
                    GUI::MenuItem("Open");

                    GUI::EndMenu();
                }

				if (GUI::BeginMenu("Edit"))
				{
					GUI::MenuItem("Copy");
					GUI::MenuItem("Paste");

					GUI::EndMenu();
				}

                GUI::EndMenuBar();
            }

            static bool shown1 = true;
            if (shown1)
            {
                GUI::BeginWindow("My Window", &shown1);
                
                if (GUI::Button("Click Me"))
                {
                    CE_LOG(Info, All, "Clicked!");
                }

				{
					GUI::BeginGroup();
					GUI::Button("HStack 0"); GUI::SameLine();
					GUI::Button("HStack 1"); GUI::SameLine();
					GUI::BeginGroup();
					GUI::Button("HStack 2 v0");
					GUI::Button("HStack 2 v1");
					GUI::Text("My Text");
					GUI::EndGroup(); GUI::SameLine();
					GUI::Button("HStack 3");
					GUI::EndGroup();
				}
                
                GUI::EndWindow();
            }

			static bool demoWindow = false;
			if (demoWindow)
			{
				GUI::ShowDemoWindow(&demoWindow);
			}
            
			if (window != nullptr)
				window->RenderGUI();
        }
        GUI::EndWindow();

        cmdList->ImGuiRender();
        cmdList->End();

        cmdList->ImGuiPlatformUpdate();

        if (RHI::gDynamicRHI->ExecuteCommandList(cmdList))
        {
            RHI::gDynamicRHI->PresentViewport(cmdList);
        }
    }
}

