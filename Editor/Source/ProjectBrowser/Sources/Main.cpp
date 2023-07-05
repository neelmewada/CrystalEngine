
#include "Main.h"

#include "ProjectBrowser.private.h"

int main(int argc, const char** argv)
{
	gLoop.PreInit(argc, argv);

	gLoop.Init();

	gLoop.PostInit();

	CERegisterModuleTypes();

	gLoop.RunLoop();

	CEDeregisterModuleTypes();

	gLoop.PreShutdown();
	gLoop.Shutdown();

	return 0;
}
