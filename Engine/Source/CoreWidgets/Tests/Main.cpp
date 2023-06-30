

#include "CoreWidgets.h"

#include <iostream>
#include <any>

#include <gtest/gtest.h>

#define TEST_BEGIN BeginTest();

#define TEST_END EndTest();

#define LOG(x) std::cout << x << std::endl
#define LOG_ERR(x) std::cerr << x << std::endl;


using namespace CE;
using namespace CE::Widgets;

void BeginTest()
{
    CE::gProjectPath = PlatformDirectories::GetLaunchDir();
    CE::gProjectName = "CoreWidgets_Test";
    CE::ModuleManager::Get().LoadModule("Core");
    CE::ModuleManager::Get().LoadModule("CoreApplication");
    CE::ModuleManager::Get().LoadModule("CoreMedia");
    CE::ModuleManager::Get().LoadModule("CoreRHI");
    CE::ModuleManager::Get().LoadModule("VulkanRHI");
    CE::ModuleManager::Get().LoadModule("CoreGUI");
    CE::ModuleManager::Get().LoadModule("CoreWidgets");
}

void EndTest()
{
    CE::ModuleManager::Get().UnloadModule("CoreWidgets");
    CE::ModuleManager::Get().UnloadModule("CoreGUI");
    CE::ModuleManager::Get().UnloadModule("VulkanRHI");
    CE::ModuleManager::Get().UnloadModule("CoreRHI");
    CE::ModuleManager::Get().UnloadModule("CoreMedia");
    CE::ModuleManager::Get().UnloadModule("CoreApplication");
    CE::ModuleManager::Get().UnloadModule("Core");
}

/**********************************************
*   Performance
*/

TEST(Style, Parser)
{
    TEST_BEGIN;
    
    
    
    TEST_END;
}

