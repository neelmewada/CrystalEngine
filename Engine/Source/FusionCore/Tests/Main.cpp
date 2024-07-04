
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
		windowInfo.hidden = false;
		windowInfo.windowFlags = PlatformWindowFlags::DestroyOnClose;

		PlatformWindow* window = app->InitMainWindow("MainWindow", 1024, 768, windowInfo);
		window->SetBorderless(true);

		InputManager::Get().Initialize(app);
	}

	gDynamicRHI = new Vulkan::VulkanRHI();

	gDynamicRHI->Initialize();
	gDynamicRHI->PostInitialize();

	if (gui)
	{
		RHI::FrameSchedulerDescriptor desc{};
		desc.numFramesInFlight = 2;
		FrameScheduler::Create(desc);
	}

	RPISystem::Get().Initialize();

	FusionApplication* fApp = FusionApplication::Get();

	FusionInitInfo initInfo = {};
	fApp->Initialize(initInfo);

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

	FusionApplication* fApp = FusionApplication::Get();

	fApp->PreShutdown();
	fApp->Shutdown();
	delete fApp;

	if (gui)
	{
		delete FrameScheduler::Get();
	}

	RPISystem::Get().Shutdown();

	gDynamicRHI->PreShutdown();
	gDynamicRHI->Shutdown();
	delete gDynamicRHI; gDynamicRHI = nullptr;

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
	ModuleManager::Get().UnloadModule("VulkanRHI");
	ModuleManager::Get().UnloadModule("CoreRHI");
	ModuleManager::Get().UnloadModule("CoreShader");
	ModuleManager::Get().UnloadModule("CoreMedia");
	ModuleManager::Get().UnloadModule("CoreInput");
	ModuleManager::Get().UnloadModule("CoreApplication");
	ModuleManager::Get().UnloadModule("Core");
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
			auto primaryBtn = CreateObject<FButtonCustomStyle>(rootStyle, "PrimaryButton");
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
			auto windowCloseBtn = CreateObject<FButtonCustomStyle>(rootStyle, "WindowWindowClose");
			rootStyle->Add("Button.WindowClose", windowCloseBtn);

			windowCloseBtn->background = Color::Clear();
			windowCloseBtn->hoveredBackground = Color::RGBA(161, 57, 57);
			windowCloseBtn->hoveredBackground = Color::RGBA(196, 43, 28);
			windowCloseBtn->pressedBackground = Color::RGBA(178, 43, 30);
			windowCloseBtn->borderColor = Color::Clear();
			windowCloseBtn->borderWidth = 0.0f;
			windowCloseBtn->cornerRadius = Vec4();
			windowCloseBtn->contentMoveY = 0;

			auto windowControlBtn = CreateObject<FButtonCustomStyle>(rootStyle, "WindowControlButton");
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

			primaryTextInput->background = Color::RGBA(15, 15, 15);
			primaryTextInput->borderColor = Color::RGBA(60, 60, 60);
			primaryTextInput->editingBorderColor = Color::RGBA(0, 112, 224);
			primaryTextInput->hoverBorderColor = Color::RGBA(74, 74, 74);
			primaryTextInput->borderWidth = 1.0f;
			primaryTextInput->cornerRadius = Vec4(5, 5, 5, 5);

			GetDefaultWidget<FTextInput>()
				.Style(rootStyle, "TextInput.Primary");
		}

		{
			auto primaryComboBox = CreateObject<FComboBoxPlainStyle>(rootStyle, "PrimaryComboBoxStyle");
			rootStyle->Add("ComboBox.Primary", primaryComboBox);

			primaryComboBox->background = Color::RGBA(15, 15, 15);
			primaryComboBox->borderColor = Color::RGBA(60, 60, 60);
			primaryComboBox->pressedBorderColor = primaryComboBox->hoverBorderColor = Color::RGBA(74, 74, 74);
			primaryComboBox->borderWidth = 1.0f;
			primaryComboBox->cornerRadius = Vec4(5, 5, 5, 5);

			auto primaryComboBoxItem = CreateObject<FComboBoxItemPlainStyle>(rootStyle, "PrimaryComboBoxItemStyle");
			rootStyle->Add("ComboBoxItem.Primary", primaryComboBoxItem);

			primaryComboBoxItem->background = Color::Clear();
			primaryComboBoxItem->hoverBackground = Color::RGBA(0, 112, 224);
			primaryComboBoxItem->selectedBackground = Color::Clear();
			primaryComboBoxItem->selectedShape = FShapeType::RoundedRect;
			primaryComboBoxItem->shapeCornerRadius = Vec4(1, 1, 1, 1) * 3;
			primaryComboBoxItem->selectedBorderColor = primaryComboBoxItem->hoverBackground;
			primaryComboBoxItem->borderWidth = 1.0f;

			GetDefaultWidget<FComboBox>()
				.ItemStyle(primaryComboBoxItem)
				.Style(rootStyle, "ComboBox.Primary");

			auto primaryComboBoxPopup = CreateObject<FComboBoxPopupPlainStyle>(rootStyle, "PrimaryComboBoxPopupStyle");
			rootStyle->Add("ComboBoxPopup.Primary", primaryComboBoxPopup);

			primaryComboBoxPopup->background = Color::RGBA(26, 26, 26);
			primaryComboBoxPopup->borderWidth = 0.0f;

			GetDefaultWidget<FComboBoxPopup>()
				.Style(rootStyle, "ComboBoxPopup.Primary");
		}
	}

	PlatformWindow* mainWindow = PlatformApplication::Get()->GetMainWindow();
	mainWindow->Show();

	FNativeContext* nativeContext = FNativeContext::Create(mainWindow, "TestWindow", rootContext);
	rootContext->AddChildContext(nativeContext);

	RenderingTestWidget* mainWidget;
	FAssignNewOwned(RenderingTestWidget, mainWidget, nativeContext);

	nativeContext->SetOwningWidget(mainWidget);

	auto exposedTick = [&]
		{
			FusionApplication::Get()->Tick(true);
		};

	DelegateHandle handle = PlatformApplication::Get()->AddTickHandler(exposedTick);

	mainWidget->comboBox->ApplyStyle();

	int frameCounter = 0;

	while (!IsEngineRequestingExit())
	{
		auto curTime = clock();
		deltaTime = (f32)(curTime - previousTime) / CLOCKS_PER_SEC;

		if (frameCounter == 1)
		{
			mainWidget->model->UpdateMemoryFootprint();
		}

		// App & Input Tick
		PlatformApplication::Get()->Tick();
		InputManager::Get().Tick();

		FusionApplication::Get()->Tick();

		previousTime = curTime;
		frameCounter++;
	}

	PlatformApplication::Get()->RemoveTickHandler(handle);

	styleManager->DeregisterStyleSet(rootStyle->GetName());

	TEST_END_GUI;
}

