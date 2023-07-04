
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
	
	Array<void*> fontHandles{};

	cmdList->InitImGui(&fontConfig, fontHandles);
    
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
	background: rgb(36, 36, 36);
	padding: 5 25 5 5;
	flex-direction: column;
	align-items: flex-start;
}

CLabel {
	background: rgb(120, 0, 0);
	foreground: white;
	min-height: 50;
	min-width: 50%;
	text-align: middle-center;
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
    
	for (int i = 0; i < 8; i++)
	{
		auto label = CreateWidget<CLabel>(window, "Label");
		label->SetInteractable(true);
		label->SetText("This is a label");
		//label->AddStyleProperty(CStylePropertyType::Background, Color(0, 0.5f, 0, 1));
		//label->AddStyleProperty(CStylePropertyType::Background, Color(0.5f, 0.5f, 0, 1), CStateFlag::Hovered);
		//label->AddStyleProperty(CStylePropertyType::Background, Color(0.5f, 0.5f, 0.5f, 1), CStateFlag::Pressed);
		//label->AddStyleProperty(CStylePropertyType::MinHeight, 25);
	}

	/*auto button = CreateWidget<CButton>(window, "Button");
	button->SetText("Click Me");
	button->AddStyleProperty(CStylePropertyType::Padding, Vec4(10, 10, 10, 10));
	button->AddStyleProperty(CStylePropertyType::Background, Color(0, 0.5f, 0, 1));
	button->AddStyleProperty(CStylePropertyType::Background, Color(0.5f, 0.5f, 0, 1), CStateFlag::Hovered);
	button->AddStyleProperty(CStylePropertyType::Background, Color(0.5f, 0.5f, 0.5f, 1), CStateFlag::Pressed);

	Object::Bind(button, MEMBER_FUNCTION(CButton, OnButtonClicked), []
		{
			CE_LOG(Info, All, "Button clicked!");
		});

	auto inputField = CreateWidget<CTextInput>(window, "TextInput");
	inputField->SetHint("Type here...");
	inputField->AddStyleProperty(CStylePropertyType::Background, Color::FromRGBA32(15, 15, 15));
	inputField->AddStyleProperty(CStylePropertyType::BorderWidth, 1);
	inputField->AddStyleProperty(CStylePropertyType::BorderRadius, Vec4(1, 1, 1, 1));
	inputField->AddStyleProperty(CStylePropertyType::Padding, Vec4(1, 1, 1, 1) * 3);
	inputField->AddStyleProperty(CStylePropertyType::Height, 30);
	inputField->AddStyleProperty(CStylePropertyType::BorderColor, Color::FromRGBA32(42, 42, 42));
	inputField->AddStyleProperty(CStylePropertyType::ForegroundColor, Color::White());*/

	/*gStyleManager->globalStyle.AddProperty(CStylePropertyType::Background, Color::FromRGBA32(36, 36, 36), CStateFlag::Default, CSubControl::Window);

	gStyleManager->globalStyle.AddProperty(CStylePropertyType::Background, Color::FromRGBA32(47, 47, 47), CStateFlag::Default, CSubControl::Header);
	gStyleManager->globalStyle.AddProperty(CStylePropertyType::Background, Color::FromRGBA32(60, 60, 60), CStateFlag::Hovered, CSubControl::Header);
	gStyleManager->globalStyle.AddProperty(CStylePropertyType::Background, Color::FromRGBA32(70, 70, 70), CStateFlag::Pressed, CSubControl::Header);

	gStyleManager->globalStyle.AddProperty(CStylePropertyType::Background, Color::FromRGBA32(15, 15, 15), CStateFlag::Default, CSubControl::Frame);
	gStyleManager->globalStyle.AddProperty(CStylePropertyType::Background, Color::FromRGBA32(28, 28, 28), CStateFlag::Hovered, CSubControl::Frame);
	gStyleManager->globalStyle.AddProperty(CStylePropertyType::Background, Color::FromRGBA32(40, 40, 40), CStateFlag::Pressed, CSubControl::Frame);

	gStyleManager->globalStyle.AddProperty(CStylePropertyType::BorderWidth, 1.5f, CStateFlag::Default, CSubControl::None);
	gStyleManager->globalStyle.AddProperty(CStylePropertyType::BorderRadius, 1.0f, CStateFlag::Default, CSubControl::Frame);
	gStyleManager->globalStyle.AddProperty(CStylePropertyType::BorderColor, Color::FromRGBA32(45, 45, 45), CStateFlag::Default, CSubControl::None);

	gStyleManager->globalStyle.AddProperty(CStylePropertyType::Background, Color::White(), CStateFlag::Default, CSubControl::CheckMark);
	gStyleManager->globalStyle.AddProperty(CStylePropertyType::Background, Color(0.08f, 0.08f, 0.09f, 0.83f), CStateFlag::Default, CSubControl::Tab);
	gStyleManager->globalStyle.AddProperty(CStylePropertyType::Background, Color(0.33f, 0.34f, 0.36f, 0.83f), CStateFlag::Hovered, CSubControl::Tab);
	gStyleManager->globalStyle.AddProperty(CStylePropertyType::Background, Color(0.23f, 0.23f, 0.24f, 1.00f), CStateFlag::Active, CSubControl::Tab);
	gStyleManager->globalStyle.AddProperty(CStylePropertyType::Background, Color(0.08f, 0.08f, 0.09f, 1.00f), CStateFlag::Unfocused, CSubControl::Tab);

	gStyleManager->globalStyle.AddProperty(CStylePropertyType::Background, Color(0.08f, 0.08f, 0.09f, 1.00f), CStateFlag::Default, CSubControl::TitleBar);
	gStyleManager->globalStyle.AddProperty(CStylePropertyType::Background, Color(0.08f, 0.08f, 0.09f, 1.00f), CStateFlag::Active, CSubControl::TitleBar);
	gStyleManager->globalStyle.AddProperty(CStylePropertyType::Background, Color(0.00f, 0.00f, 0.00f, 0.51f), CStateFlag::Collapsed, CSubControl::TitleBar);
	gStyleManager->globalStyle.AddProperty(CStylePropertyType::ForegroundColor, Color::White());

	auto& buttonSelector = gStyleManager->AddStyleGroup("CButton");
	buttonSelector.AddProperty(CStylePropertyType::Background, Color(0.25f, 0.25f, 0.25f, 1.00f), CStateFlag::Default);
	buttonSelector.AddProperty(CStylePropertyType::Background, Color(0.38f, 0.38f, 0.38f, 1.00f), CStateFlag::Hovered);
	buttonSelector.AddProperty(CStylePropertyType::Background, Color(0.67f, 0.67f, 0.67f, 0.39f), CStateFlag::Pressed);

	auto& tableSelector = gStyleManager->GetStyleGroup("CTableView");
	tableSelector.AddProperty(CStylePropertyType::Background, Color(0.20f, 0.20f, 0.20f, 1.0f), CStateFlag::Default, CSubControl::Header);
	tableSelector.AddProperty(CStylePropertyType::Background, Color(0.23f, 0.23f, 0.23f, 1.0f), CStateFlag::Hovered, CSubControl::Header);
	tableSelector.AddProperty(CStylePropertyType::Background, Color(0.26f, 0.26f, 0.26f, 1.0f), CStateFlag::Pressed, CSubControl::Header);
	tableSelector.AddProperty(CStylePropertyType::Background, Color(0.00f, 0.00f, 0.00f, 0.00f), CStateFlag::Default, CSubControl::TableRowEven);
	tableSelector.AddProperty(CStylePropertyType::Background, Color(1.00f, 1.00f, 1.00f, 0.06f), CStateFlag::Default, CSubControl::TableRowOdd);
	tableSelector.AddProperty(CStylePropertyType::Background, Color::FromRGBA32(30, 30, 30), CStateFlag::Default, CSubControl::TableBorderInner);
	
	auto& collapsibleSelector = gStyleManager->GetStyleGroup("CCollapsibleSection");
	collapsibleSelector.AddProperty(CStylePropertyType::BorderColor, Color::FromRGBA32(30, 30, 30), CStateFlag::Default, CSubControl::Header);
	collapsibleSelector.AddProperty(CStylePropertyType::Padding, Vec4(4, 4, 4, 4), CStateFlag::Default, CSubControl::Header);
	collapsibleSelector.AddProperty(CStylePropertyType::BorderRadius, Vec4(), CStateFlag::Default, CSubControl::Header);

	auto& textInputSelector = gStyleManager->GetStyleGroup(".TextField");
	textInputSelector.AddProperty(CStylePropertyType::Padding, Vec4(5, 6, 5, 6));
	textInputSelector.AddProperty(CStylePropertyType::Width, CStyleValue(100, true));
	textInputSelector.AddProperty(CStylePropertyType::BorderRadius, Vec4(1, 1, 1, 1) * 1.5f);

	window = CreateWidget<CWindow>(nullptr, "TestWindow");
	window->SetWidgetFlags(WidgetFlags::None);
    window->SetTitle("Test Window");
	window->GetStyle().AddProperty(CStylePropertyType::ForegroundColor, Color::White());

	CStackLayout* horizontal = CreateWidget<CStackLayout>(window, "Horizontal");
	horizontal->SetDirection(CStackDirection::Horizontal);
    
    CLabel* label = CreateWidget<CLabel>(horizontal, "MyLabel");
    label->SetText("Label Text");
    label->GetStyle().AddProperty(CStylePropertyType::ForegroundColor, Color::White());
	label->GetStyle().AddProperty(CStylePropertyType::Background, Color(0.5f, 0.5f, 0.0f, 1.0f));
	label->GetStyle().AddProperty(CStylePropertyType::Background, Color(0.0f, 1.0f, 0.0f, 1.0f), CStateFlag::Hovered);
	label->GetStyle().AddProperty(CStylePropertyType::Background, Color(0.0f, 0.0f, 1.0f, 1.0f), CStateFlag::Pressed);

	CLabel* label2 = CreateWidget<CLabel>(horizontal, "Label2");
	label2->SetText("This is a long form label widget! And this is another sentence in the same label.");
	
	CStackLayout* vertical = CreateWidget<CStackLayout>(horizontal, "Vertical");
	vertical->SetDirection(CStackDirection::Vertical);

	CLabel* vertLabel = CreateWidget<CLabel>(vertical, "VertLabel");
	vertLabel->SetText("Vert Label");

	CButton* vertButton = CreateWidget<CButton>(vertical, "VertButton");
	vertButton->SetText("Vert Button");

	CButton* button = CreateWidget<CButton>(window, "MyButton");
	button->SetText("Click Me!");
	button->GetStyle().AddProperty(CStylePropertyType::BorderRadius, Vec4(5, 5, 5, 5));
	button->GetStyle().AddProperty(CStylePropertyType::Background, Color(0.5f, 0.5f, 0.0f, 1.0f));
	button->GetStyle().AddProperty(CStylePropertyType::Background, Color(0.0f, 1.0f, 0.0f, 1.0f), CStateFlag::Hovered);
	button->GetStyle().AddProperty(CStylePropertyType::Background, Color(0.0f, 0.0f, 1.0f, 1.0f), CStateFlag::Pressed);
	button->GetStyle().AddProperty(CStylePropertyType::Width, CStyleValue(50, true));
	button->GetStyle().AddProperty(CStylePropertyType::MinHeight, 30);
	
	Object::Bind(button, MEMBER_FUNCTION(CButton, OnButtonClicked), []()
		{
			CE_LOG(Info, All, "Button Clicked from lambda!");
		});

	// Collapsible
	CCollapsibleSection* section = CreateWidget<CCollapsibleSection>(window, "MySection");
	section->SetTitle("MY SECTION");
	section->SetIndent(1);

	CTableView* table = CreateWidget<CTableView>(section, "MyTable");
	MyTableModel* model = CreateObject<MyTableModel>(table, "TableModel");
	table->SetModel(model);
	table->SetTableFlags(CTableFlags::ResizeableColumns | CTableFlags::ScrollY | CTableFlags::BordersInnerV);
	table->GetStyle().AddProperty(CStylePropertyType::Height, 200);

	CButton* childBtn0 = CreateWidget<CButton>(section, "Button0");
	childBtn0->SetText("Btn 0");
	CButton* childBtn1 = CreateWidget<CButton>(section, "Button1");
	childBtn1->SetText("Btn 1");
	CButton* childBtn2 = CreateWidget<CButton>(section, "Button2");
	childBtn2->SetText("Btn 2");
	CButton* childBtn3 = CreateWidget<CButton>(section, "Button3");
	childBtn3->SetText("Btn 3");

	CCheckbox* checkbox = CreateWidget<CCheckbox>(window, "Check0");
	checkbox->SetMixedState();
	
	Object::Bind(checkbox, MEMBER_FUNCTION(CCheckbox, OnValueChanged), [](s8 newValue)
		{
			CE_LOG(Info, All, "Checkbox changed: {}", newValue);
		});

	CTextInput* textInput = CreateWidget<CTextInput>(window, "TextInput");
	textInput->SetHint("Type here...");

	Object::Bind(textInput, MEMBER_FUNCTION(CTextInput, OnValueChanged), [](String string)
		{
			CE_LOG(Info, All, "TextInput changed: {}", string);
		});

	CFloatInput* floatInput = CreateWidget<CFloatInput>(window, "FloatInput");
	CIntInput* intInput = CreateWidget<CIntInput>(window, "IntInput");

	Object::Bind(intInput, MEMBER_FUNCTION(CIntInput, OnValueChanged), [](s64 newValue)
		{
			CE_LOG(Info, All, "CIntInput changed: {}", newValue);
		});

	CGridLayout* gridLayout = CreateWidget<CGridLayout>(window, "GridView");

	for (int i = 0; i < 32; i++)
	{
		CButton* gridBtn = CreateWidget<CButton>(gridLayout, "GridButton");
		gridBtn->SetText(String::Format("Label: {}", i));
	}*/
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

