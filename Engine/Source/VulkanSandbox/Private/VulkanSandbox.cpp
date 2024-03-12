
#include "VulkanSandbox.h"

#include "VulkanSandbox.private.h"

int main(int argc, char** argv)
{	
	CE::SandboxLoop sandbox{};

	sandbox.PreInit(argc, argv);

	sandbox.Init();

	CERegisterModuleTypes();

	sandbox.PostInit();

	sandbox.RunLoop();

	sandbox.PreShutdown();

	CEDeregisterModuleTypes();

	sandbox.Shutdown();

	return 0;
}

