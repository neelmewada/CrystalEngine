#pragma once

namespace CE
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

		void Tick();

		void PreShutdown();
		void Shutdown();

	private:

		clock_t previousTime{};

		PlatformWindow* secondWindow = nullptr;
		bool secondWindowHidden = false;

		VulkanSandbox* main = nullptr;

		DelegateHandle tickDelegateHandle = 0;
	};
    
} // namespace CE
