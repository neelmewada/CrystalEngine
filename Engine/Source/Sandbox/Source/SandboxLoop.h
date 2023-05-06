#pragma once

#include "Core.h"

class SandboxLoop
{
public:
    SandboxLoop() = default;
    ~SandboxLoop() = default;

    void PreInit(int argc, const char** argv);

    void Init();

    void PostInit();

    void PreShutdown();
    void Shutdown();
};