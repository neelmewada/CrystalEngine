#pragma once

#include "Core.h"
#include "CoreApplication.h"
#include "CoreRHI.h"
#include "CoreGUI.h"

class SandboxLoop
{
public:
    SandboxLoop() = default;
    ~SandboxLoop() = default;

    void PreInit(int argc, const char** argv);
    void Init();
    void PostInit();

    void RunLoop();

    void PreShutdown();
    void Shutdown();

private:

    void AppPreInit();
    void AppInit();

    void AppPreShutdown();
    void AppShutdown();

    void LoadStartupCoreModules();
    void LoadCoreModules();

    // Application
    CE::PlatformApplication* app = nullptr;

    // RHI
    CE::RHI::Viewport* viewport = nullptr;
    CE::RHI::GraphicsCommandList* cmdList = nullptr;
};
