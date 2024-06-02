
#include "FusionCore.h"
#include "VulkanRHI.h"

#include <gtest/gtest.h>

#include "FusionCore_Test.private.h"

#define TEST_BEGIN TestBegin(false)
#define TEST_END TestEnd(false)

#define TEST_BEGIN_GUI TestBegin(true)
#define TEST_END_GUI TestEnd(true)

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

	Logger::Initialize();
}

static void TestEnd(bool gui)
{
	Logger::Shutdown();

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

	ModuleManager::Get().UnloadModule("CoreRPI");
	ModuleManager::Get().UnloadModule("VulkanRHI");
	ModuleManager::Get().UnloadModule("CoreRHI");
	ModuleManager::Get().UnloadModule("CoreShader");
	ModuleManager::Get().UnloadModule("CoreMedia");
	ModuleManager::Get().UnloadModule("CoreInput");
	ModuleManager::Get().UnloadModule("CoreApplication");
	ModuleManager::Get().UnloadModule("Core");
}


TEST(FusionCore, Declarative)
{
	TEST_BEGIN;

	FusionApplication* app = FusionApplication::Get();

	TEST_END;
}

