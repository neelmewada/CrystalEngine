
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
		ComplexWidget* widget;

		FAssignNewOwned(widget, app, ComplexWidget);

		EXPECT_NE(widget->rootBox, nullptr);
		EXPECT_EQ(widget->rootBox->GetDirection(), FStackBoxDirection::Vertical);
		EXPECT_EQ(widget->m_ChildSlot->GetOwner(), widget);
		EXPECT_EQ(widget->m_ChildSlot->GetChild(), widget->rootBox);
		EXPECT_EQ(widget->m_ChildSlot, widget->rootBox->m_Parent);

		EXPECT_EQ(widget->rootBox->GetSlotCount(), 1);
		EXPECT_EQ(widget->rootBox->GetSlot(0)->GetOuter(), widget->rootBox);
		EXPECT_TRUE(widget->rootBox->GetSlot(0)->GetChild()->IsOfType<FStackBox>());

		FStackBox* stack1 = (FStackBox*)widget->rootBox->GetSlot(0)->GetChild();
		EXPECT_EQ(stack1->GetDirection(), FStackBoxDirection::Horizontal);
		EXPECT_EQ(stack1->GetSlotCount(), 1);
		EXPECT_EQ(stack1->GetSubObjectCount(), 1);

		FSlot* stack1Slot0 = stack1->GetSlot(0);

		EXPECT_TRUE(stack1Slot0->GetChild()->IsOfType<FStackBox>());

		FStackBox* stack2 = (FStackBox*)stack1->GetSlot(0)->GetChild();
		EXPECT_EQ(stack2->GetDirection(), FStackBoxDirection::Vertical);
		EXPECT_EQ(stack2->GetParent(), stack1->GetSlot(0));
		EXPECT_EQ(stack2->GetSlotCount(), 2);

		// Try Removing slots

		bool removeResult = stack1->RemoveSlot(stack1Slot0);
		EXPECT_TRUE(removeResult);
		EXPECT_EQ(stack1->GetSlotCount(), 0);
		EXPECT_EQ(stack1->GetSubObjectCount(), 0);
		EXPECT_EQ(stack1Slot0->GetOwner(), nullptr);
		EXPECT_EQ(stack1Slot0->GetOuter(), nullptr);
		EXPECT_EQ(stack1Slot0->GetChild(), nullptr);
		EXPECT_EQ(stack2->GetParent(), nullptr);
		EXPECT_EQ(stack2->GetOuter(), widget);

		// You must destroy the FSlot object, or it will be leaked.
		// Otherwise, use DestroySlot() instead of RemoveSlot()
		stack1Slot0->Destroy(); stack1Slot0 = nullptr;

		// Stack2's content should remain intact, we only removed the parent slot!
		EXPECT_EQ(stack2->GetDirection(), FStackBoxDirection::Vertical);
		EXPECT_EQ(stack2->GetSlotCount(), 2);

		// Add Stack2 back inside stack1 by creating a new slot
		// And also add 1 extra item inside Stack2, amounting a total of 3 slots thereafter

		stack1->AddSlot(FStackBox::Slot()
			(
				FAssign(stack2)
				.Direction(FStackBoxDirection::Horizontal)
				+ FStackBox::Slot()
				.Padding(3)
				(
					FNewOwned(app, FNullWidget)
				)
			)
		);

		EXPECT_EQ(stack1->GetSlotCount(), 1);

		EXPECT_EQ(stack2->GetDirection(), FStackBoxDirection::Horizontal);
		EXPECT_EQ(stack2->GetSlotCount(), 3);
		EXPECT_EQ(stack2->GetSlot(0)->GetPadding(), Vec4(1, 1, 1, 1));
		EXPECT_EQ(stack2->GetSlot(1)->GetPadding(), Vec4(2, 2, 2, 2));
		EXPECT_EQ(stack2->GetSlot(2)->GetPadding(), Vec4(3, 3, 3, 3));
		EXPECT_EQ(stack2->GetParent(), stack1->GetSlot(0));
		EXPECT_EQ(stack2->GetOuter(), widget);

		stack1Slot0 = stack1->GetSlot(0);

		// Destroying the widget object directly should remove its parent slot too!
		stack1->Destroy(); stack1 = nullptr;

		EXPECT_EQ(widget->rootBox->GetSlotCount(), 0);

		widget->Destroy();
	}

	TEST_END;
}

TEST(FusionCore, Layout)
{
	TEST_BEGIN;

	FusionApplication* app = FusionApplication::Get();

	FFusionContext* rootContext = CreateObject<FFusionContext>(app, "RootContext");
	app->SetRootContext(rootContext);

	int frameCount = 0;
	constexpr int MaxNumFrames = 4;



	while (frameCount < MaxNumFrames)
	{
		app->Tick();

		frameCount++;
	}

	TEST_END;
}
