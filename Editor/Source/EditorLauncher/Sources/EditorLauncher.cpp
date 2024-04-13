
#include "EditorLauncher.h"

int GuardedMain(int argc, char** argv)
{
	gEditorLoop.PreInit(argc, argv);

	gEditorLoop.Init();

	gEditorLoop.PostInit();

	gEditorLoop.RunLoop();

	gEditorLoop.PreShutdown();
	gEditorLoop.Shutdown();

	return 0;
}

