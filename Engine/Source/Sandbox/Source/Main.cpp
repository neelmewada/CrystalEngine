
#include "Core.h"
#include "SandboxLoop.h"

SandboxLoop loop{};

int main(int argc, const char** argv)
{
    loop.PreInit(argc, argv);

    loop.Init();

    loop.PreShutdown();
    loop.Shutdown();
    return 0;
}

