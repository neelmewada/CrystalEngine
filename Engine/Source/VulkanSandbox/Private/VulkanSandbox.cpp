
#include "VulkanSandbox.h"

int main(int argc, char** argv)
{
	CE::SandboxLoop sandbox{};

	sandbox.PreInit(argc, argv);

	sandbox.Init();

	sandbox.PostInit();

	sandbox.RunLoop();

	sandbox.PreShutdown();

	sandbox.Shutdown();

	return 0;
}

