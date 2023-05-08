
#include "Core.h"
#include "SandboxLoop.h"

static SandboxLoop loop{};

int main(int argc, const char** argv)
{
    loop.PreInit(argc, argv);

    loop.Init();

    loop.PostInit();

    loop.RunLoop();

    loop.PreShutdown();
    loop.Shutdown();
    return 0;
}

