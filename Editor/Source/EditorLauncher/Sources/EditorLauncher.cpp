
#include "EditorLauncher.h"

#include "EditorLauncher.private.h"

int GuardedMain(int argc, char** argv)
{
	gEditorLoop.PreInit(argc, argv);

	gEditorLoop.Init();

	gEditorLoop.PostInit();

	CERegisterModuleTypes();

	gEditorLoop.RunLoop();

	CEDeregisterModuleTypes();

	gEditorLoop.PreShutdown();
	gEditorLoop.Shutdown();

	return 0;
}

