
#include "SandboxLoop.h"

using namespace CE;

void SandboxLoop::PreInit(int argc, const char** argv)
{
    // Setup before loading any module
}

void SandboxLoop::Init()
{
    // Load Startup Modules
    ModuleManager::Get().LoadModule("Core");
    
}

void SandboxLoop::PostInit()
{
    // Setup after loading important modules
}

void SandboxLoop::PreShutdown()
{
    
}

void SandboxLoop::Shutdown()
{
    ModuleManager::Get().UnloadModule("Core");
}

