
#include "GameLauncher.h"


int GuardedMain(int argc, char** argv)
{
	gGameLoop.PreInit(argc, argv);

	gGameLoop.Init();
	gGameLoop.PostInit();

	gGameLoop.RunLoop();

	gGameLoop.PreShutdown();
	gGameLoop.Shutdown();

    return 0;
}

