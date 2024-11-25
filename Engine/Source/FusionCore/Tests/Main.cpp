
#include "FusionCore.h"
#include "VulkanRHI.h"

#include "FusionCoreTest.h"

#include <gtest/gtest.h>

#include "FusionCore_Test.private.h"

#define TEST_BEGIN TestBegin(false)
#define TEST_END TestEnd(false)

#define TEST_BEGIN_GUI TestBegin(true)
#define TEST_END_GUI TestEnd(true)

static CE::JobManager* gJobManager = nullptr;
static CE::JobContext* gJobContext = nullptr;

using namespace CE;

static void TestBegin(bool gui)
{
	gProjectPath = PlatformDirectories::GetLaunchDir();
	gProjectName = MODULE_NAME;

	ModuleManager::Get().LoadModule("Core");
	ModuleManager::Get().LoadModule("CoreApplication");
	ModuleManager::Get().LoadModule("CoreInput");
	ModuleManager::Get().LoadModule("CoreMedia");
	ModuleManager::Get().LoadModule("CoreShader");
	ModuleManager::Get().LoadModule("CoreRHI");
	ModuleManager::Get().LoadModule("VulkanRHI");
	ModuleManager::Get().LoadModule("CoreRPI");
	ModuleManager::Get().LoadModule("FusionCore");
	CERegisterModuleTypes();

	PlatformApplication* app = PlatformApplication::Get();

	app->Initialize();

	if (gui)
	{
		PlatformWindowInfo windowInfo{};
		windowInfo.fullscreen = windowInfo.hidden = windowInfo.maximised = windowInfo.resizable = false;
		windowInfo.resizable = true;
		windowInfo.hidden = true;
		windowInfo.windowFlags = PlatformWindowFlags::DestroyOnClose;

		u32 w = 1024, h = 768;
#if PLATFORM_MAC
		w = 540; h = 400;
#endif

		PlatformWindow* window = app->InitMainWindow("MainWindow", w, h, windowInfo);
		window->SetBorderless(true);

		InputManager::Get().Initialize(app);
	}

	RHI::gDynamicRHI = new Vulkan::VulkanRHI();

	RHI::gDynamicRHI->Initialize();
	RHI::gDynamicRHI->PostInitialize();

	if (gui)
	{
		RHI::FrameSchedulerDescriptor desc{};
		desc.numFramesInFlight = 2;
		RHI::FrameScheduler::Create(desc);
	}

	RPI::RPISystem::Get().Initialize();

	FusionApplication* fApp = FusionApplication::Get();

	FusionInitInfo initInfo = {};
	initInfo.assetLoader = nullptr;
	fApp->Initialize(initInfo);

	RendererSystem::Get().Init();

	JobManagerDesc desc{};
	desc.defaultTag = JOB_THREAD_WORKER;
	desc.totalThreads = 0; // auto set optimal number of threads
	desc.threads.Resize(4, { .threadName = "FusionThread", .tag = JOB_THREAD_FUSION });

	gJobManager = new JobManager("JobSystemManager", desc);
	gJobContext = new JobContext(gJobManager);
	JobContext::PushGlobalContext(gJobContext);

	Logger::Initialize();
}
static void TestEnd(bool gui)
{
	Logger::Shutdown();

	gJobManager->DeactivateWorkersAndWait();

	JobContext::PopGlobalContext();
	delete gJobContext;
	gJobContext = nullptr;
	delete gJobManager;
	gJobManager = nullptr;

	RendererSystem::Get().Shutdown();

	FusionApplication* fApp = FusionApplication::Get();

	fApp->PreShutdown();
	fApp->Shutdown();
	fApp->BeginDestroy();
	
	if (gui)
	{
		delete RHI::FrameScheduler::Get();
	}

	RPI::RPISystem::Get().Shutdown();

	RHI::gDynamicRHI->PreShutdown();
	RHI::gDynamicRHI->Shutdown();
	delete RHI::gDynamicRHI; RHI::gDynamicRHI = nullptr;

	PlatformApplication* app = PlatformApplication::Get();

	if (gui)
	{
		InputManager::Get().Shutdown(app);
	}

	app->PreShutdown();
	app->Shutdown();
	delete app;

	CEDeregisterModuleTypes();
	ModuleManager::Get().UnloadModule("CoreRPI");
	ModuleManager::Get().UnloadModule("FusionCore");
	ModuleManager::Get().UnloadModule("VulkanRHI");
	ModuleManager::Get().UnloadModule("CoreRHI");
	ModuleManager::Get().UnloadModule("CoreShader");
	ModuleManager::Get().UnloadModule("CoreMedia");
	ModuleManager::Get().UnloadModule("CoreInput");
	ModuleManager::Get().UnloadModule("CoreApplication");
	ModuleManager::Get().UnloadModule("Core");
}

static void DoPaint(FusionRenderer2* renderer)
{
	renderer->Begin();

	renderer->SetBrush(FBrush(Color::Black()));
	renderer->FillRect(Rect::FromSize(100, 100, 200, 200));

	renderer->PushChildCoordinateSpace(Vec2(100, 100));
	{
		FPen pen = Color::Blue();
		pen.SetThickness(2.0f);
		renderer->SetPen(pen);
		renderer->SetBrush(Color::Cyan());

		renderer->FillRect(Rect::FromSize(30, 30, 100, 60), Vec4(5, 10, 15, 20));
		renderer->StrokeRect(Rect::FromSize(30, 30, 100, 60), Vec4(5, 10, 15, 20));

		f32 angle = 0;

		renderer->PushChildCoordinateSpace(Matrix4x4::Translation(Vec3(50, 50)) * Matrix4x4::Angle(angle) * Matrix4x4::Translation(Vec3(-50, -50)));

		renderer->SetBrush(Color::Red());
		renderer->PathLineTo(Vec2(50, 50));
		//renderer->PathArcTo(Vec2(50, 50), 50, Math::ToRadians(0), Math::ToRadians(90));
		renderer->PathArcToFast(Vec2(50, 50), 50, 0, 3);
		renderer->PathFillStroke(true);

		renderer->PopChildCoordinateSpace();
	}
	renderer->PopChildCoordinateSpace();

	renderer->FillCircle(Vec2(300, 300), 25);
	renderer->StrokeCircle(Vec2(300, 300), 25);

	renderer->End();
}

TEST(FusionCore, Rendering)
{
	TEST_BEGIN_GUI;
	using namespace RenderingTests;

	f32 deltaTime = 0;
	clock_t previousTime = {};

	FusionApplication* app = FusionApplication::Get();
	FStyleManager* styleManager = app->GetStyleManager();

	FRootContext* rootContext = FusionApplication::Get()->GetRootContext();

	FStyleSet* rootStyle = CreateObject<FStyleSet>(rootContext, "RootStyleSet");
	styleManager->RegisterStyleSet(rootStyle->GetName(), rootStyle);
	rootContext->SetDefaultStyleSet(rootStyle);

	// - Styling -
	{
		{
			auto primaryBtn = CreateObject<FCustomButtonStyle>(rootStyle, "PrimaryButton");
			rootStyle->Add("Button.Primary", primaryBtn);

			primaryBtn->background = Color::RGBA(56, 56, 56);
			primaryBtn->hoveredBackground = Color::RGBA(95, 95, 95);
			primaryBtn->pressedBackground = Color::RGBA(50, 50, 50);
			primaryBtn->cornerRadius = Vec4(1, 1, 1, 1) * 5;
			primaryBtn->borderColor = Color::RGBA(24, 24, 24);
			primaryBtn->borderWidth = 1.0f;

			GetDefaultWidget<FButton>()
				.Padding(Vec4(10, 5, 10, 5))
				.Style(rootStyle, "Button.Primary")
				;
			GetDefaultWidget<FTextButton>()
				.Padding(Vec4(10, 5, 10, 5))
				.Style(rootStyle, "Button.Primary")
				;

			GetDefaultWidget<FScrollBox>()
				.ScrollBarBackground(Color::RGBA(50, 50, 50))
				.ScrollBarBrush(Color::RGBA(60, 60, 60))
				;
		}

		{
			auto windowCloseBtn = CreateObject<FCustomButtonStyle>(rootStyle, "WindowWindowClose");
			rootStyle->Add("Button.WindowClose", windowCloseBtn);

			windowCloseBtn->background = Color::Clear();
			windowCloseBtn->hoveredBackground = Color::RGBA(161, 57, 57);
			windowCloseBtn->hoveredBackground = Color::RGBA(196, 43, 28);
			windowCloseBtn->pressedBackground = Color::RGBA(178, 43, 30);
			windowCloseBtn->borderColor = Color::Clear();
			windowCloseBtn->borderWidth = 0.0f;
			windowCloseBtn->cornerRadius = Vec4();
			windowCloseBtn->contentMoveY = 0;

			auto windowControlBtn = CreateObject<FCustomButtonStyle>(rootStyle, "WindowControlButton");
			rootStyle->Add("Button.WindowControl", windowControlBtn);

			windowControlBtn->background = Color::Clear();
			windowControlBtn->hoveredBackground = Color::RGBA(58, 58, 58);
			windowControlBtn->pressedBackground = Color::RGBA(48, 48, 48);
			windowControlBtn->borderColor = Color::Clear();
			windowControlBtn->borderWidth = 0.0f;
			windowControlBtn->cornerRadius = Vec4();
			windowControlBtn->contentMoveY = 0;
		}
		
		{
			auto primaryTextInput = CreateObject<FTextInputPlainStyle>(rootStyle, "PrimaryTextInput");
			rootStyle->Add("TextInput.Primary", primaryTextInput);

			primaryTextInput->background = primaryTextInput->hoverBackground = primaryTextInput->editingBackground = Color::RGBA(15, 15, 15);
			primaryTextInput->borderColor = Color::RGBA(60, 60, 60);
			primaryTextInput->editingBorderColor = Color::RGBA(0, 112, 224);
			primaryTextInput->hoverBorderColor = Color::RGBA(74, 74, 74);
			primaryTextInput->borderWidth = 1.0f;
			primaryTextInput->cornerRadius = Vec4(5, 5, 5, 5);

			GetDefaultWidget<FTextInput>()
				.Style(rootStyle, "TextInput.Primary");
		}

		{
			auto primaryComboBox = CreateObject<FComboBoxStyle>(rootStyle, "PrimaryComboBoxStyle");
			rootStyle->Add("ComboBox.Primary", primaryComboBox);

			primaryComboBox->background = Color::RGBA(15, 15, 15);
			primaryComboBox->borderColor = Color::RGBA(60, 60, 60);
			primaryComboBox->pressedBorderColor = primaryComboBox->hoverBorderColor = Color::RGBA(74, 74, 74);
			primaryComboBox->borderWidth = 1.0f;
			primaryComboBox->cornerRadius = Vec4(5, 5, 5, 5);

			auto primaryComboBoxItem = CreateObject<FComboBoxItemStyle>(rootStyle, "PrimaryComboBoxItemStyle");
			rootStyle->Add("ComboBoxItem.Primary", primaryComboBoxItem);

			primaryComboBoxItem->background = Color::Clear();
			primaryComboBoxItem->hoverBackground = Color::RGBA(0, 112, 224);
			primaryComboBoxItem->selectedBackground = Color::Clear();
			primaryComboBoxItem->selectedShape = FShapeType::RoundedRect;
			primaryComboBoxItem->shapeCornerRadius = Vec4(1, 1, 1, 1) * 3;
			primaryComboBoxItem->selectedBorderColor = primaryComboBoxItem->hoverBackground.GetFillColor();
			primaryComboBoxItem->borderWidth = 1.0f;

			GetDefaultWidget<FComboBox>()
				.ItemStyle(primaryComboBoxItem)
				.Style(rootStyle, "ComboBox.Primary");

			auto primaryComboBoxPopup = CreateObject<FComboBoxPopupStyle>(rootStyle, "PrimaryComboBoxPopupStyle");
			rootStyle->Add("ComboBoxPopup.Primary", primaryComboBoxPopup);

			primaryComboBoxPopup->background = Color::RGBA(26, 26, 26);
			primaryComboBoxPopup->borderWidth = 0.0f;

			GetDefaultWidget<FComboBoxPopup>()
				.Style(rootStyle, "ComboBoxPopup.Primary");
		}

		{
			auto primaryTabView = CreateObject<FTabViewStyle>(rootStyle, "TabViewStyle");
			rootStyle->Add("TabView.Primary", primaryTabView);

			primaryTabView->tabItemActiveBackground = Color::RGBA(50, 50, 50);
			primaryTabView->tabItemHoverBackground = Color::RGBA(40, 40, 40);
			primaryTabView->tabItemBackground = Color::RGBA(36, 36, 36);
			primaryTabView->tabItemShape = FRoundedRectangle(3, 3, 0, 0);

			GetDefaultWidget<FTabView>()
				.Style(rootStyle, "TabView.Primary");
		}

		{
			auto menuStyle = CreateObject<FMenuPopupStyle>(rootStyle, "MenuPopupStyle");
			rootStyle->Add("MenuPopup.Primary", menuStyle);

			menuStyle->background = Color::RGBA(56, 56, 56);
			menuStyle->itemPadding = Vec4(10, 5, 10, 5);
			menuStyle->itemHoverBackground = Color::RGBA(0, 112, 224);

			GetDefaultWidget<FMenuPopup>()
				.Style(rootStyle, "MenuPopup.Primary");

			auto menuBarStyle = CreateObject<FMenuBarStyle>(rootStyle, "MenuBarStyle");
			rootStyle->Add("MenuBar.Primary", menuBarStyle);

			menuBarStyle->background = Color::Black();
			menuBarStyle->itemPadding = Vec4(5, 5, 5, 5);
			menuBarStyle->itemHoverBackground = Color::RGBA(0, 112, 224);

			GetDefaultWidget<FMenuBar>()
				.Style(rootStyle, "MenuBar.Primary");
		}
	}

	PlatformWindow* mainWindow = PlatformApplication::Get()->GetMainWindow();

	FNativeContext* nativeContext = FNativeContext::Create(mainWindow, "TestWindow", rootContext);
	rootContext->AddChildContext(nativeContext);

	RenderingTestWidget* mainWidget;
	FAssignNewOwned(RenderingTestWidget, mainWidget, nativeContext);

	nativeContext->SetOwningWidget(mainWidget);

	auto exposedTick = [&]
		{
			FusionApplication::Get()->SetExposed();

			FusionApplication::Get()->Tick();

			RendererSystem::Get().Render();
		};

	DelegateHandle handle = PlatformApplication::Get()->AddTickHandler(exposedTick);

	if (mainWidget->comboBox)
	{
		mainWidget->comboBox->ApplyStyle();
	}

	mainWindow->Show();

	int frameCounter = 0;

	while (!IsEngineRequestingExit())
	{
		auto curTime = clock();
		deltaTime = (f32)(curTime - previousTime) / CLOCKS_PER_SEC;

		FusionApplication::Get()->ResetExposed();

		// App & Input Tick
		PlatformApplication::Get()->Tick();
		InputManager::Get().Tick();

		FusionApplication::Get()->Tick();

		DoPaint(nativeContext->GetPainter()->GetRenderer());

		RendererSystem::Get().Render();

		previousTime = curTime;
		frameCounter++;
	}

	PlatformApplication::Get()->RemoveTickHandler(handle);

	styleManager->DeregisterStyleSet(rootStyle->GetName());

	TEST_END_GUI;
}

