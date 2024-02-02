#pragma once

namespace CE::Sandbox
{
	class VulkanSandbox;

	class SandboxLoop
	{
	public:
		SandboxLoop();
		virtual ~SandboxLoop();

		void PreInit(int argc, char** argv);
		void Init();
		void PostInit();

		void RunLoop();

		void PreShutdown();
		void Shutdown();

	private:

		clock_t previousTime{};

		VulkanSandbox* main = nullptr;
	};
    
} // namespace CE
