
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
		windowInfo.windowFlags = PlatformWindowFlags::DestroyOnClose;

		app->InitMainWindow("MainWindow", 1024, 768, windowInfo);

		InputManager::Get().Initialize(app);
	}

	gDynamicRHI = new Vulkan::VulkanRHI();

	gDynamicRHI->Initialize();
	gDynamicRHI->PostInitialize();

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

TEST(FusionCore, Layout)
{
	TEST_BEGIN;
	using namespace LayoutTests;

	FusionApplication* app = FusionApplication::Get();

	FFusionContext* rootContext = CreateObject<FFusionContext>(app, "RootContext");
	app->SetRootContext(rootContext);

	LayoutTestWidget* rootWidget;

	FAssignNewOwned(LayoutTestWidget, rootWidget, rootContext);

	rootContext->SetOwningWidget(rootWidget);

	Vec2 availSize = Vec2(500, 700);

	rootContext->SetAvailableSize(availSize);
	rootContext->DoLayout();

	auto rootBox = rootWidget->rootBox;

	EXPECT_EQ(rootBox->GetChild(0), rootWidget->hStack1);
	EXPECT_EQ(rootWidget->GetComputedSize(), availSize);

	EXPECT_EQ(rootBox->GetPadding(), Vec4(1, 1, 1, 1) * 5);
	EXPECT_EQ(rootBox->GetContentHAlign(), HAlign::Fill);

	EXPECT_EQ(rootBox->GetComputedPosition(), Vec2(0, 0));
	EXPECT_EQ(rootBox->GetComputedSize(), availSize);

	// HStack1

	{
		auto hStack1 = rootWidget->hStack1;

		EXPECT_EQ(hStack1->GetComputedPosition(), Vec2(5, 5));
		EXPECT_EQ(hStack1->GetComputedSize().width, availSize.width - 10);

		EXPECT_EQ(hStack1->GetChildCount(), 6);

		TerminalWidget* child0 = (TerminalWidget*)hStack1->GetChild(0);
		EXPECT_EQ(child0->GetComputedPosition(), Vec2(5, 12.5f));
		EXPECT_EQ(child0->GetComputedSize(), Vec2(10, 10));

		TerminalWidget* child1 = (TerminalWidget*)hStack1->GetChild(1);
		EXPECT_EQ(child1->GetComputedPosition(), Vec2(15, 2.5f));
		EXPECT_EQ(child1->GetComputedSize(), Vec2(10, 30));

		f32 remainingWidth = availSize.width - 40;

		TerminalWidget* child2 = (TerminalWidget*)hStack1->GetChild(2);
		EXPECT_EQ(child2->GetComputedPosition(), Vec2(25, 2.5f));
		EXPECT_EQ(child2->GetComputedSize(), Vec2(remainingWidth / 5, 30));

		TerminalWidget* child3 = (TerminalWidget*)hStack1->GetChild(3);
		EXPECT_EQ(child3->GetComputedPosition(), Vec2(117, 2.5f));
		EXPECT_EQ(child3->GetComputedSize(), Vec2(remainingWidth * 2.0f / 5.0f, 15));

		TerminalWidget* child4 = (TerminalWidget*)hStack1->GetChild(4);
		EXPECT_EQ(child4->GetComputedPosition(), Vec2(301, 10));
		EXPECT_EQ(child4->GetComputedSize(), Vec2(remainingWidth / 5.0f, 15));

		TerminalWidget* child5 = (TerminalWidget*)hStack1->GetChild(5);
		EXPECT_EQ(child5->GetComputedPosition(), Vec2(393, 17.5f));
		EXPECT_EQ(child5->GetComputedSize(), Vec2(remainingWidth / 5.0f, 15));
	}

	// HStack2

	{
		auto hStack2 = rootWidget->hStack2;
		Vec2 stackSize = hStack2->GetComputedSize();

		TerminalWidget* child0 = (TerminalWidget*)hStack2->GetChild(0);
		EXPECT_EQ(child0->GetComputedPosition(), Vec2(0, 10));

		constexpr f32 fixedWidthPart = 10;
		constexpr f32 totalMargins = 5 + 5;
		f32 remainingSize = stackSize.width - fixedWidthPart - totalMargins;

		TerminalWidget* child1 = (TerminalWidget*)hStack2->GetChild(1);
		EXPECT_EQ(child1->GetComputedPosition(), Vec2(15, 0));
		EXPECT_EQ(child1->GetComputedSize(), Vec2(remainingSize * 0.25f, 30));

		TerminalWidget* child2 = (TerminalWidget*)hStack2->GetChild(2);
		EXPECT_EQ(child2->GetComputedPosition(), Vec2(fixedWidthPart + totalMargins + remainingSize * 0.25f, 10));
		EXPECT_EQ(child2->GetComputedSize(), Vec2(remainingSize * 0.75f, 10));
	}

	// HStack3

	{
		auto hStack3 = rootWidget->hStack3;
		Vec2 stackSize = hStack3->GetComputedSize();

		// (5 + 5) is vertical margin of rootBox
		EXPECT_EQ(stackSize, Vec2(20, availSize.height - 35 - 30 - (5 + 5)));
		EXPECT_EQ(hStack3->GetComputedPosition().x, (availSize.x - stackSize.x) * 0.5f);

		TerminalWidget* child0 = (TerminalWidget*)hStack3->GetChild(0);
		EXPECT_EQ(child0->GetComputedPosition(), Vec2(0, 0));

		TerminalWidget* child1 = (TerminalWidget*)hStack3->GetChild(1);
		EXPECT_EQ(child1->GetComputedPosition(), Vec2(10, 0));

	}

	TEST_END;
}

TEST(FusionCore, Rendering)
{
	TEST_BEGIN_GUI;
	using namespace RenderingTests;

	f32 deltaTime = 0;
	clock_t previousTime = {};

	FusionApplication* app = FusionApplication::Get();

	FFusionContext* rootContext = CreateObject<FFusionContext>(app, "RootContext");
	app->SetRootContext(rootContext);

	FStyleManager* rootStyle = CreateObject<FStyleManager>(rootContext, "RootStyleManager");
	rootContext->SetStyleManager(rootStyle);

	// - Styling -
	{
		{
			auto primaryBtn = CreateObject<FPlainButtonStyle>(rootStyle, "PrimaryButton");
			rootStyle->RegisterStyle("Button.Primary", primaryBtn);

			primaryBtn->background = Color::RGBA(56, 56, 56);
			primaryBtn->hoveredBackground = Color::RGBA(95, 95, 95);
			primaryBtn->pressedBackground = Color::RGBA(50, 50, 50);
		}

	}

	PlatformWindow* mainWindow = PlatformApplication::Get()->GetMainWindow();

	FNativeContext* nativeContext = FNativeContext::Create(mainWindow, "TestWindow", rootContext);
	rootContext->AddChildContext(nativeContext);

	while (!IsEngineRequestingExit())
	{
		auto curTime = clock();
		deltaTime = (f32)(curTime - previousTime) / CLOCKS_PER_SEC;

		// App & Input Tick
		PlatformApplication::Get()->Tick();
		InputManager::Get().Tick();

		app->Tick();

		previousTime = curTime;
	}

	TEST_END_GUI;
}

