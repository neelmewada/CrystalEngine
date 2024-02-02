
#include "VulkanSandbox.h"

#include "Resource.h"
#include "VulkanSandbox.private.h"

int main(int argc, char** argv)
{	
	CE::Sandbox::SandboxLoop sandbox{};

	sandbox.PreInit(argc, argv);

	sandbox.Init();

	CERegisterModuleTypes();
	CERegisterModuleResources();

	sandbox.PostInit();

	sandbox.RunLoop();

	sandbox.PreShutdown();

	CEDeregisterModuleResources();
	CEDeregisterModuleTypes();

	sandbox.Shutdown();

	return 0;
}

