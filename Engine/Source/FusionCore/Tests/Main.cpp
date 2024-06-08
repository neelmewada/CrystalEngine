
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

TEST(FusionCore, Construction)
{
	TEST_BEGIN;
	using namespace ConstructTests;

	FusionApplication* app = FusionApplication::Get();

	{
		ComplexWidget* widget = CreateObject<ComplexWidget>(app, "ComplexWidget");

		EXPECT_NE(widget->rootBox, nullptr);
		EXPECT_EQ(widget->rootBox->GetDirection(), FStackBoxDirection::Vertical);

		EXPECT_EQ(widget->rootBox->GetName(), "RootStackBox");
		EXPECT_TRUE(widget->rootBox->GetChild(0)->IsOfType<FStackBox>());
		EXPECT_EQ(widget->rootBox->GetPadding(), Vec4(10, 10, 10, 10));

		FStackBox* stack0 = (FStackBox*)widget->rootBox->GetChild(0);
		EXPECT_EQ(stack0->GetName(), "Stack0");
		EXPECT_EQ(stack0->GetDirection(), FStackBoxDirection::Horizontal);

		FStackBox* stack0_0 = (FStackBox*)stack0->GetChild(0);
		EXPECT_EQ(stack0_0->GetName(), "Stack0_0");
		EXPECT_EQ(stack0_0->GetDirection(), FStackBoxDirection::Vertical);
		EXPECT_EQ(stack0_0->GetPadding(), Vec4(5, 2, 5, 2));

		EXPECT_EQ(stack0_0->GetChildCount(), 2);

		EXPECT_EQ(stack0_0->GetChild(0)->GetPadding(), Vec4(1, 1, 1, 1));
		EXPECT_EQ(stack0_0->GetChild(1)->GetPadding(), 2 * Vec4(1, 1, 1, 1));

		widget->Destroy();
	}

	TEST_END;
}

/*
TEST(FusionCore, Layout)
{
	TEST_BEGIN;
	using namespace LayoutTests;

	FusionApplication* app = FusionApplication::Get();

	FFusionContext* rootContext = CreateObject<FFusionContext>(app, "RootContext");
	app->SetRootContext(rootContext);

	LayoutTestWidget* rootWidget = nullptr;

	FAssignNewOwned(rootWidget, rootContext, LayoutTestWidget);
	rootContext->SetOwningWidget(rootWidget);

	rootContext->SetAvailableSize(Vec2(500, 700));

	rootContext->DoLayout();

	TEST_END;
}*/
