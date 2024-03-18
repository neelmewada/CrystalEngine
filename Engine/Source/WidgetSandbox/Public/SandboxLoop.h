#pragma once

namespace CE
{
	class WidgetSandbox;

	class SandboxLoop
	{
	public:
		SandboxLoop();
		virtual ~SandboxLoop();

		void PreInit(int argc, char** argv);
		void Init();
		void PostInit();

		void RunLoop();

		void AlternativeTick();

		void PreShutdown();
		void Shutdown();

	private:

		clock_t previousTime{};
		f32 deltaTime = 0.0f;

		WidgetSandbox* main = nullptr;

		DelegateHandle tickDelegateHandle = 0;
	};
    
} // namespace CE
