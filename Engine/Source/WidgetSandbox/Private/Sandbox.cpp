#include "WidgetSandbox.h"

using namespace CE::Widgets;

namespace CE
{
	static int counter = 0;
	static RPI::RPISystem& rpiSystem = RPI::RPISystem::Get();

	void WidgetSandbox::Init(PlatformWindow* window)
	{
		auto prevTime = clock();

		localCounter = counter++;
		rpiSystem.Initialize();
		
		RHI::FrameSchedulerDescriptor desc{};
		desc.numFramesInFlight = 2;
		
		scheduler = RHI::FrameScheduler::Create(desc);

		auto assetManager = gEngine->GetAssetManager();

		auto renderer2dShader = assetManager->LoadAssetAtPath<CE::Shader>("/Engine/Assets/Shaders/2D/SDFGeometry");

		auto fontAsset = assetManager->LoadAssetAtPath<Font>("/Engine/Assets/Fonts/Roboto");
		auto poppinsFont = assetManager->LoadAssetAtPath<Font>("/Engine/Assets/Fonts/Poppins");

		auto atlasData = fontAsset->GetAtlasData();

		CApplicationInitInfo appInitInfo{};
		appInitInfo.draw2dShader = renderer2dShader->GetOrCreateRPIShader(0);
		appInitInfo.defaultFont = atlasData;
		appInitInfo.defaultFontName = "Roboto";
		appInitInfo.numFramesInFlight = 2;
		appInitInfo.scheduler = scheduler;
		appInitInfo.resourceLoader = this;

		CApplication::Get()->Initialize(appInitInfo);

		CApplication::Get()->RegisterFont("Poppins", poppinsFont->GetAtlasData());

		mainWindow = window;
		mainWindow->SetBorderless(true);
		platformWindows.Add(mainWindow);

		//RHI::SwapChainDescriptor swapChainDesc{};
		//swapChainDesc.imageCount = 2;
		//swapChainDesc.preferredFormats = { RHI::Format::R8G8B8A8_UNORM, RHI::Format::B8G8R8A8_UNORM };

		//swapChain = RHI::gDynamicRHI->CreateSwapChain(mainWindow, swapChainDesc);

		mainWindow->SetMinimumSize(Vec2i(1280, 720));
		mainWindow->GetDrawableWindowSize(&width, &height);

		PlatformApplication::Get()->AddMessageHandler(this);

		CApplication* app = CApplication::Get();

		app->LoadGlobalStyleSheet(PlatformDirectories::GetLaunchDir() / "Engine/Styles/Style.css");
		
		InitFontAtlas();
		InitWidgets();

		BuildFrameGraph();
		CompileFrameGraph();

		f32 timeTaken = ((f32)(clock() - prevTime)) / CLOCKS_PER_SEC;

		CE_LOG(Info, All, "Initialization time: {} seconds", timeTaken);
	}

	void WidgetSandbox::InitWidgets()
	{
		mainDockSpace = CreateWindow<CDockSpace>(MODULE_NAME, mainWindow);

		mainDockWindow = CreateObject<CDockWindow>(mainDockSpace->GetRootDockSplit(), MODULE_NAME);
		mainDockWindow->SetAsMainWindow(true);
		{
			CMenuItem* fileMenuItem = CreateObject<CMenuItem>(mainDockWindow, "FileMenuItem");
			fileMenuItem->SetText("File");
			{
				CMenu* fileMenu = CreateObject<CMenu>(fileMenuItem, "FileMenu");

				CMenuItem* newFile = CreateObject<CMenuItem>(fileMenu, "New");
				newFile->SetText("New");

				CMenuItem* open = CreateObject<CMenuItem>(fileMenu, "Open");
				open->SetText("Open");

				CMenuItem* openAs = CreateObject<CMenuItem>(fileMenu, "OpenAs");
				openAs->SetText("Open As...");

				CMenuItem* openRecent = CreateObject<CMenuItem>(fileMenu, "OpenRecent");
				openRecent->SetText("Open Recent");
				{
					CMenu* openRecentMenu = CreateObject<CMenu>(openRecent, "OpenRecentMenu");

					CMenuItem* proj1 = CreateObject<CMenuItem>(openRecentMenu, "Proj1");
					proj1->SetText("Project 1");

					CMenuItem* proj2 = CreateObject<CMenuItem>(openRecentMenu, "Proj2");
					proj2->SetText("Project 2");

					CMenuItem* proj3 = CreateObject<CMenuItem>(openRecentMenu, "Proj3");
					proj3->SetText("Project 3");

					CMenuItem* otherProjects = CreateObject<CMenuItem>(openRecentMenu, "Others");
					otherProjects->SetText("Other Projects");
					{
						CMenu* otherProjectsMenu = CreateObject<CMenu>(otherProjects, "OthersMenu");

						CMenuItem* other1 = CreateObject<CMenuItem>(otherProjectsMenu, "Other1");
						other1->SetText("Other 1");

						CMenuItem* other2 = CreateObject<CMenuItem>(otherProjectsMenu, "Other2");
						other2->SetText("Other 2");
					}
				}

				CMenuItem* save = CreateObject<CMenuItem>(fileMenu, "Save");
				save->SetText("Save");

				CMenuItem* saveAs = CreateObject<CMenuItem>(fileMenu, "SaveAs");
				saveAs->SetText("Save As...");

				CMenuItem* exit = CreateObject<CMenuItem>(fileMenu, "Exit");
				exit->SetText("Exit");

				Object::Bind(exit, MEMBER_FUNCTION(CMenuItem, OnMenuItemClicked), []
					{
						RequestEngineExit("USER_QUIT");
					});
			}

			CMenuItem* editMenuItem = CreateObject<CMenuItem>(mainDockWindow, "EditMenuItem");
			editMenuItem->SetText("Edit");
			{
				CMenu* editMenu = CreateObject<CMenu>(editMenuItem, "EditMenu");

				CMenuItem* newFile = CreateObject<CMenuItem>(editMenu, "New");
				newFile->SetText("New");

				CMenuItem* open = CreateObject<CMenuItem>(editMenu, "Open");
				open->SetText("Open");

				CMenuItem* openAs = CreateObject<CMenuItem>(editMenu, "OpenAs");
				openAs->SetText("Open As...");

				CMenuItem* save = CreateObject<CMenuItem>(editMenu, "Save");
				save->SetText("Save");

				CMenuItem* saveAs = CreateObject<CMenuItem>(editMenu, "SaveAs");
				saveAs->SetText("Save As...");

				CMenuItem* exit = CreateObject<CMenuItem>(editMenu, "Exit");
				exit->SetText("Exit");
			}

			CMenuItem* toolsMenuItem = CreateObject<CMenuItem>(mainDockWindow, "ToolsMenuItem");
			toolsMenuItem->SetText("Tools");
			{
				CMenu* menu = CreateObject<CMenu>(toolsMenuItem, "ToolsMenu");
			}

			testPopup = CreateObject<CPopup>(CApplication::Get(), "TestPopup");
			testPopup->SetTitle("Sample Popup");
			{
				CLabel* popupLabel = CreateObject<CLabel>(testPopup, "TitleLabel");
				popupLabel->SetText("This is a popup");

				CLabel* descLabel = CreateObject<CLabel>(testPopup, "Label");
				descLabel->SetText("This is a long description text. This is the second sentence.");
			}

			CMenuItem* helpMenuItem = CreateObject<CMenuItem>(mainDockWindow, "HelpMenuItem");
			helpMenuItem->SetText("Help");
			{
				CMenu* helpMenu = CreateObject<CMenu>(helpMenuItem, "HelpMenu");

				CMenuItem* about = CreateObject<CMenuItem>(helpMenu, "About");
				about->SetText("About");
			}
		}

		AssetManager* assetManager = gEngine->GetAssetManager();

		topViewTex = assetManager->LoadAssetAtPath<CE::Texture>("/Engine/Assets/Sandbox/TopView");

		CWidget* toolBar = CreateObject<CWidget>(mainDockWindow, "ToolBar");
		
		minorDockSpace = CreateObject<CDockSpace>(nullptr, "MinorDockSpace");
		minorDockSpace->SetDockType(CDockType::Minor);
		mainDockWindow->AddSubWidget(minorDockSpace);

		minorDockSpace->Split(0.25f, CDockSplitDirection::Horizontal, "SplitLeft", "SplitRight");
		auto parentSplit = minorDockSpace->GetRootDockSplit();
		auto left = parentSplit->GetSplit(0);
		auto right = parentSplit->GetSplit(1);

		minorDockSpace->Split(right, 0.5f, CDockSplitDirection::Vertical, "SplitRightTop", "SplitRightBottom");
		auto rightTop = right->GetSplit(0);
		auto rightBottom = right->GetSplit(1);

		secondDockWindow = CreateObject<CDockWindow>(left, "Second");
		{
			CWidget* imageWidget1 = CreateObject<CWidget>(secondDockWindow, "ImageWidget");

			CTabWidget* tabWidget = CreateObject<CTabWidget>(secondDockWindow, "TabWidget");
			{
				CTabWidgetContainer* tab1 = CreateObject<CTabWidgetContainer>(tabWidget, "Tab1");
				tab1->SetTitle("Tab 1");
				{
					CListWidget* listWidget = CreateObject<CListWidget>(tab1, "ListWidget");
					{
						for (int i = 0; i < 32; ++i)
						{
							CListWidgetItem* listItem = CreateObject<CListWidgetItem>(listWidget, "ListItem");

							CLabel* titleLabel = CreateObject<CLabel>(listItem, "TitleLabel");
							titleLabel->SetText(String::Format("Empty Project {}", i));

							CLabel* descLabel = CreateObject<CLabel>(listItem, "DescLabel");
							descLabel->SetText("Create an empty project!");
						}
					}
				}

				CTabWidgetContainer* tab2 = CreateObject<CTabWidgetContainer>(tabWidget, "Tab2");
				tab2->SetTitle("Tab 2");
				{
					CButton* button = CreateObject<CButton>(tab2, "Button");
					button->SetAlternateStyle(false);
					button->SetText("Click Me 2");
				}
			}
		}

		thirdDockWindow = CreateObject<CDockWindow>(rightTop, "Third");
		fourthDockWindow = CreateObject<CDockWindow>(rightBottom, "Fourth");
		fifthDockWindow = CreateObject<CDockWindow>(rightBottom, "Fifth");


		CButton* newBtn = CreateObject<CButton>(thirdDockWindow, "Button");
		newBtn->SetText("New Button");

		CLabel* label = CreateObject<CLabel>(thirdDockWindow, "Label");
		label->SetText("This is a label");
		
		CTextInput* textInput = CreateObject<CTextInput>(thirdDockWindow, "TextInputDemo");
		textInput->SetText("This is text box with a large text value.");
		textInput->SetAsPassword(false);

		CLabel* testLabel = CreateObject<CLabel>(thirdDockWindow, "TestLabel");
		testLabel->SetText("This is a test label");

		CWidget* imageWidget = CreateObject<CWidget>(thirdDockWindow, "ImageWidget");

		CWidget* imageWidget2 = CreateObject<CWidget>(thirdDockWindow, "ImageWidget2");
		CWidget* imageWidget3 = CreateObject<CWidget>(thirdDockWindow, "ImageWidget2");
		CWidget* imageWidget4 = CreateObject<CWidget>(thirdDockWindow, "ImageWidget2");

		Object::Bind(newBtn, MEMBER_FUNCTION(CButton, OnButtonLeftClicked), [this]
			{
				testPopup->Show(Vec2i(500, 500), Vec2i(400, 150));
			});

		CTreeView* treeView = CreateObject<CTreeView>(fourthDockWindow, "TreeView");

		CFileSystemModel* model = CreateObject<CFileSystemModel>(treeView, "FileSystemModel");
		model->SetRootDirectory(PlatformDirectories::GetLaunchDir()); // Test path

		treeView->SetModel(model);

		treeView->SetColumnResizable(0, 0.5f);
		treeView->SetColumnResizable(1, 0.5f);
	}

	void WidgetSandbox::DestroyWidgets()
	{
	}

	void WidgetSandbox::Tick(f32 deltaTime)
	{
		if (destroyed)
			return;

		u32 curWidth = 0, curHeight = 0;
		mainWindow->GetDrawableWindowSize(&curWidth, &curHeight);

		if (width != curWidth || height != curHeight)
		{
			rebuild = recompile = true;
			width = curWidth;
			height = curHeight;
		}

		if (rebuild)
		{
			rebuild = false;
			recompile = true;

			BuildFrameGraph();
		}

		if (recompile)
		{
			recompile = false;
			resubmit = true;

			CompileFrameGraph();
		}

		u32 imageIndex = scheduler->BeginExecution();

		if (imageIndex >= RHI::Limits::MaxSwapChainImageCount)
		{
			rebuild = recompile = true;
			return;
		}

		SubmitWork(imageIndex);

		scheduler->EndExecution();
	}

	void WidgetSandbox::Shutdown()
	{
		DestroyWidgets();

		delete scheduler;

		PlatformApplication::Get()->RemoveMessageHandler(this);

		rpiSystem.Shutdown();
	}

	RPI::Texture* WidgetSandbox::LoadImage(const Name& assetPath)
	{
		AssetManager* assetManager = gEngine->GetAssetManager();
		CE::Texture* texture = assetManager->LoadAssetAtPath<CE::Texture>(assetPath);
		if (!texture)
			return nullptr;
		return texture->GetRpiTexture();
	}

	void WidgetSandbox::InitFontAtlas()
	{
		AssetManager* assetManager = gEngine->GetAssetManager();

		auto prevTime = clock();

		auto timeTaken = ((f32)(clock() - prevTime)) / CLOCKS_PER_SEC;
	}

	void WidgetSandbox::BuildFrameGraph()
	{
		rebuild = false;
		recompile = true;

		scheduler->BeginFrameGraph();
		{
			CApplication::Get()->BuildFrameGraph();
		}
		scheduler->EndFrameGraph();
	}

	void WidgetSandbox::CompileFrameGraph()
	{
		recompile = false;
		scheduler->Compile();

		RHI::TransientMemoryPool* pool = scheduler->GetTransientPool();
		RHI::MemoryHeap* imageHeap = pool->GetImagePool();
		if (imageHeap != nullptr)
		{
			CE_LOG(Info, All, "Transient Image Pool: {} MB", (imageHeap->GetHeapSize() / 1024.0f / 1024.0f));
		}
	}

	void WidgetSandbox::SubmitWork(u32 imageIndex)
	{
		resubmit = false;
		drawList.Shutdown();

		RHI::DrawListMask drawListMask{};
		//drawListMask.Set(mainDockSpace->GetDrawListTag());
		CApplication::Get()->SetDrawListMasks(drawListMask);
		drawList.Init(drawListMask);

		CApplication::Get()->FlushDrawPackets(drawList, imageIndex);

		auto prevTime = clock();

		static bool firstTime = true;

		// Finalize
		drawList.Finalize();

		CApplication::Get()->SubmitDrawPackets(drawList);
		//scheduler->SetScopeDrawList(mainWindow->GetTitle(), &drawList.GetDrawListForTag(mainDockSpace->GetDrawListTag()));
	}

	void WidgetSandbox::OnWindowResized(PlatformWindow* window, u32 newWidth, u32 newHeight)
	{
		rebuild = recompile = true;

		scheduler->WaitUntilIdle();

		//if (!mainWindow->IsMinimized())
		//	swapChain->Rebuild();
	}

	void WidgetSandbox::OnWindowDestroyed(PlatformWindow* window)
	{
		rebuild = recompile = true;

		if (window == this->mainWindow)
		{
			destroyed = true;
			this->mainWindow = nullptr;
		}
	}

	void WidgetSandbox::OnWindowMinimized(PlatformWindow* window)
	{
		rebuild = recompile = true;
	}

	void WidgetSandbox::OnWindowRestored(PlatformWindow* window)
	{
		rebuild = recompile = true;
	}

	void WidgetSandbox::OnWindowCreated(PlatformWindow* window)
	{
		rebuild = recompile = true;
	}

} // namespace CE
