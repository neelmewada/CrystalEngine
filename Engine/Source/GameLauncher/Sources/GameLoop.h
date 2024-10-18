#pragma once

using namespace CE;

class GameLoop
{
public:

	void PreInit(int argc, char** argv);
	void Init();
	void PostInit();

	void ExposedTick();
	void RunLoop();

	void PreShutdown();
	void Shutdown();

private:

	void LoadProject();
	void UnloadProject();

	void InitStyles();

	void AppPreInit();
	void AppInit();

	void AppPreShutdown();
	void AppShutdown();

	void LoadStartupCoreModules();
	void LoadCoreModules();

	void LoadEngineModules();
	void UnloadEngineModules();

	// Application
	CE::PlatformApplication* app = nullptr;
	CE::PlatformWindow* mainWindow = nullptr;

	IO::Path projectPath{};

	clock_t previousTime{};
	f32 deltaTime = 0;
	DelegateHandle tickDelegateHandle = 0;
	FGameWindow* gameWindow = nullptr;
};

extern GameLoop gGameLoop;
