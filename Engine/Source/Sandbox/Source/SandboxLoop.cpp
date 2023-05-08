
#include "SandboxLoop.h"

using namespace CE;

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

    AppInit();
}

void SandboxLoop::PreShutdown()
{
    AppPreShutdown();

    ModuleManager::Get().UnloadModule("CoreMedia");
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
    app = PlatformApplication::Create();
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

void SandboxLoop::RunLoop()
{
    while (!IsEngineRequestingExit())
    {
        app->Tick();
    }
}

