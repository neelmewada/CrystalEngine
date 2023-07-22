
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
        
        for (int i = 0; i < 100; i++)
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
		return GetStaticClass<CAbstractItemCell>();
	}

	virtual void SetData(const CModelIndex& index, CWidget* widget) override
	{
        CAbstractItemCell* itemWidget = (CAbstractItemCell*)widget;
		itemWidget->SetText(String::Format("Cell Entry: {},{}", index.GetRow(), index.GetColumn()));
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
	padding: 8 25 8 10;
	flex-direction: column;
	align-items: stretch;
	row-gap: 5px;
}

CWindow.DockSpace {
	background: rgb(21, 21, 21);
}

CLabel {
	text-align: middle-center;
}

CButton {
	padding: 10px 1px;
	border-radius: 2px;
	border-width: 1.5px;
	border-color: rgb(25, 25, 25);
	background: rgb(64, 64, 64);
}
CButton:hovered {
	background: rgb(90, 90, 90);
}
CButton:pressed {
	background: rgba(50, 50, 50);
}

CButton::alternate {
	padding: 10px 1px;
	border-radius: 2px;
	border-width: 1.5px;
	border-color: rgb(25, 25, 25);
	background: rgb(0, 112, 224);
}

CButton::alternate:hovered {
	background: rgb(14, 134, 255);
}

CButton::alternate:pressed {
	background: rgb(6, 66, 126);
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

CCollapsibleSection {
	padding: 20px 0 0 0;
}

CCollapsibleSection::header {
	font-size: 18px;
	background: rgb(47, 47, 47);
	border-width: 1px;
	border-color: rgb(30, 30, 30);
	padding: 3px 3px;
	border-radius: 0px;
}

#TableView {
	height: 250px;
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

	auto section = CreateWidget<CCollapsibleSection>(window, "CollapsibleSection");
	section->SetTitle("Header Title");

	{
		auto label = CreateWidget<CLabel>(section, "LabelInside");
		label->SetText("This is a sub-label");

		auto button = CreateWidget<CButton>(section, "ButtonInside");
		button->SetText("Click me");

		auto table = CreateWidget<CTableView>(section, "TableView");
		auto tableModel = CreateObject<MyTableModel>(table, "TableModel");
		table->SetModel(tableModel);
	}
	
	auto testLabel = CreateWidget<CLabel>(window, "TestLabel");
	testLabel->SetText("This is a test label");
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

