#pragma once

using namespace CE;

class GameLoop
{
public:

	void PreInit(int argc, char** argv);
	void Init();
	void PostInit();

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

	// RHI
	CE::RHI::Viewport* viewport = nullptr;
	CE::RHI::GraphicsCommandList* cmdList = nullptr;

	IO::Path projectPath{};

	clock_t previousTime{};

};

extern GameLoop gGameLoop;
