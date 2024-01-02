
#include "Core.h"
#include "CoreMedia.h"
#include "CoreRHI.h"
#include "VulkanRHI.h"

#include "VulkanRHITest.h"

#include <gtest/gtest.h>

using namespace CE;

#define TEST_BEGIN TestBegin()
#define TEST_END TestEnd()

static void TestBegin()
{
	gProjectName = MODULE_NAME;
	gProjectPath = PlatformDirectories::GetLaunchDir();

	ModuleManager::Get().LoadModule("Core");
	ModuleManager::Get().LoadModule("CoreMedia");
	ModuleManager::Get().LoadModule("CoreRHI");
	ModuleManager::Get().LoadModule("VulkanRHI");

	RHI::gDynamicRHI = new Vulkan::VulkanRHI();

	RHI::gDynamicRHI->Initialize();
	RHI::gDynamicRHI->PostInitialize();
}

static void TestEnd()
{
	RHI::gDynamicRHI->PreShutdown();
	RHI::gDynamicRHI->Shutdown();

	delete RHI::gDynamicRHI;
	RHI::gDynamicRHI = nullptr;

	ModuleManager::Get().LoadModule("VulkanRHI");
	ModuleManager::Get().LoadModule("CoreRHI");
	ModuleManager::Get().LoadModule("CoreMedia");
	ModuleManager::Get().LoadModule("Core");	
}

TEST(VulkanRHI, ShaderResourceGroup)
{
	TEST_BEGIN;

	TEST_END;
}

