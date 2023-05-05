
#include "SandboxLoop.h"

using namespace CE;

void SandboxLoop::PreInit(int argc, const char** argv)
{
    
}

void SandboxLoop::Init()
{
    // Load Startup Modules
    ModuleManager::Get().LoadModule("Core");
    
}

void SandboxLoop::PreShutdown()
{
    
}

void SandboxLoop::Shutdown()
{
    ModuleManager::Get().UnloadModule("Core");
}

