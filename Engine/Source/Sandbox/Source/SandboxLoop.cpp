
#include "SandboxLoop.h"
#include "OpenSans.h"

using namespace CE;
using namespace CE::Widgets;

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
	Logger::SetConsoleLogLevel(LogLevel::Trace);
	Logger::SetFileDumpLogLevel(LogLevel::Trace);
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

	Array<RHI::FontDesc> fontList{};

	auto defaultFontByteSize = OpenSans_VariableFont_len;
	void* defaultFont = OpenSans_VariableFont_ttf;

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
    
    // Setup GUI
    using namespace CE::Widgets;
    
    SetupGUI();
}

void SandboxLoop::PreShutdown()
{
	//gStyleManager->PopGlobal();

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

class MyTableModel : public CE::Widgets::CDataModel
{
	CE_CLASS(MyTableModel, CE::Widgets::CDataModel)
public:
    
    struct Entry
    {
        ~Entry()
        {
            for (auto child : children)
            {
                delete child;
            }
            children.Clear();
        }
        
        String name{};
        Entry* parent = nullptr;
        Array<Entry*> children{};
    };
    
    MyTableModel()
    {
        root = new Entry();
        
        for (int i = 0; i < 20; i++)
        {
            Entry* e = new Entry();
            e->name = String("Entry: ") + i;
            if (i == 4)
            {
                Entry* c1 = new Entry();
                Entry* c2 = new Entry();
                Entry* c3 = new Entry();
                Entry* c4 = new Entry();
                
                e->children.Add(c1);
                e->children.Add(c2);
                e->children.Add(c3);
                e->children.Add(c4);
            }
            root->children.Add(e);
        }
    }
    
    virtual ~MyTableModel() { delete root; }


	// Inherited via CDataModel
	virtual u32 GetRowCount(const CModelIndex& parent) override
	{
		if (!parent.IsValid())
		{
			return root->children.GetSize();
		}
        Entry* parentEntry = (Entry*)parent.GetInternalData();
        if (parentEntry == nullptr)
            return 0;
        return parentEntry->children.GetSize();
	}

	virtual u32 GetColumnCount(const CModelIndex& parent) override
	{
		return 3;
	}

	virtual CModelIndex GetParent(const CModelIndex& index) override
	{
        if (!index.IsValid())
            return CModelIndex();
        
        Entry* data = (Entry*)index.GetInternalData();
        if (data == nullptr || data->parent == nullptr)
            return CModelIndex();
        
        Entry* parentsParent = nullptr;
        if (data->parent->parent == nullptr)
            parentsParent = root;
        else
            parentsParent = data->parent->parent;
        
        u32 row = parentsParent->children.IndexOf(data->parent);
        
        return CreateIndex(row, index.GetColumn(), data->parent);
	}

	virtual CModelIndex GetIndex(u32 row, u32 col, const CModelIndex& parent) override
	{
        Entry* parentEntry = root;
        
        if (!parent.IsValid())
        {
            parentEntry = root;
        }
        else
        {
            parentEntry = (Entry*)parent.GetInternalData();
        }
        
        if (parentEntry == nullptr)
            return CModelIndex();
        if (row >= parentEntry->children.GetSize())
            return CModelIndex();
        
		return CreateIndex(row, col, parentEntry->children[row]);
	}

	virtual ClassType* GetWidgetClass(const CModelIndex& index) override
	{
		return GetStaticClass<CLabel>();
	}

	virtual void SetData(const CModelIndex& index, CWidget* itemWidget) override
	{
		if (itemWidget->GetClass()->IsSubclassOf<CLabel>())
		{
			CLabel* label = (CLabel*)itemWidget;
			label->SetText(String::Format("Cell Entry: {},{}", index.GetRow(), index.GetColumn()));
		}
	}

	virtual bool HasHeader() override { return true; }

	virtual String GetHeaderTitle(u32 col) override
	{
		if (col == 0)
			return "Col 0";
		else if (col == 1)
			return "Col 1";
		else if (col == 2)
			return "Col 2";

		return "";
	}
    
    String GetText(const CModelIndex& index) override
    {
        return "Tree Node";
    }
    
    int GetIndentLevel(const CModelIndex &index) override
    {
        if (index.GetColumn() == 0)
            return 1;
        return 0;
    }
    
    Entry* root = nullptr;
};

CE_RTTI_CLASS(, , MyTableModel,
	CE_SUPER(CE::Widgets::CDataModel),
	CE_NOT_ABSTRACT,
	CE_ATTRIBS(),
	CE_FIELD_LIST(),
	CE_FUNCTION_LIST()
)

CE_RTTI_CLASS_IMPL(, , MyTableModel)

static const String stylesheet = R"(
CWindow {
	foreground: white;
	background: rgb(36, 36, 36);
	padding: 5 25 5 5;
	flex-direction: column;
	align-items: flex-start;
	row-gap: 5px;
}

CLabel {
	text-align: middle-center;
	background: rgb(120, 0, 0);
}

CButton {
	padding: 10px 3px;
	border-radius: 1.5px;
	border-width: 1px;
	border-color: rgb(30, 30, 30);
	background: rgb(64, 64, 64);
}
CButton:hovered {
	background: rgb(80, 80, 80);
}
CButton:pressed {
	background: rgba(100, 100, 100);
}

CTextInput {
	background: rgb(15, 15, 15);
	border-width: 1;
	border-radius: 1 1 1 1;
	border-color: rgb(42, 42, 42);
	padding: 3 3 3 3;
}
CTextInput::hint {
	foreground: rgba(255, 255, 255, 120);
}

CTabWidget {
	width: 100%;
	height: 100%;
}

CTabWidget::tab {
	padding: 10 10 10 10;
	background: rgba(22, 22, 22, 210);
}

CTabWidget::tab:hovered {
	background: rgba(90, 90, 90, 210);
}

CTabWidget::tab:active {
	background: rgba(60, 60, 60, 255);
}

CTabWidget::tab:unfocused {
	background: rgba(22, 22, 22);
}

CTabContainerWidget {
	padding: 3 0 3 0;
}

#SelectableGroup {
	width: 100%;
	height: 200px;
	background: rgb(26, 26, 26);
	padding: 0px 0px;
}

#SelectableGroup > CSelectableWidget {
	width: 100%;
	height: 35px;
    padding: 0;
}

.Selectable {
	padding: 10px 0px;
	foreground: white;
	font-size: 18;
}

.Selectable:hovered {
	background: rgb(43, 50, 58);
}
.Selectable:pressed, .Selectable:active {
	background: rgb(65, 87, 111);
}

.Selectable > CLabel {
	background: none;
	height: 100%;
	text-align: middle-left;
}
)";

void SandboxLoop::SetupGUI()
{
    using namespace CE::Widgets;

	GetStyleManager()->SetGlobalStyleSheet(stylesheet);

	window = CreateWidget<CWindow>(nullptr, "TestWindow");
	window->SetWidgetFlags(WidgetFlags::None);
	window->SetTitle("Test Window");
	window->SetAllowHorizontalScroll(true);

	
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

		//gStyleManager->PushGlobal();

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

			static bool demoWindow = true;
			if (demoWindow)
			{
				GUI::ShowDemoWindow(&demoWindow);
			}
            
			if (window != nullptr)
				window->RenderGUI();
        }
        GUI::EndWindow();

		//gStyleManager->PopGlobal();

        cmdList->ImGuiRender();
        cmdList->End();

        cmdList->ImGuiPlatformUpdate();

        if (RHI::gDynamicRHI->ExecuteCommandList(cmdList))
        {
            RHI::gDynamicRHI->PresentViewport(cmdList);
        }
    }
}

