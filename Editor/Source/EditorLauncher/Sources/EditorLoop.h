#pragma once

using namespace CE;

class EditorLoop
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

	void InitStyles();
	void ExposedTick();

	void AppPreInit();
	void AppInit();

	void AppPreShutdown();
	void AppShutdown();

	void LoadStartupCoreModules();
	void LoadCoreModules();

	void LoadEngineModules();
	void UnloadEngineModules();

	void LoadEditorModules();
	void UnloadEditorModules();

	// Application
	CE::PlatformApplication* app = nullptr;
	PlatformWindow* mainWindow = nullptr;

	IO::Path projectPath{};

	clock_t previousTime{};
	f32 deltaTime = 0.0f;

	int frameCounter = 0;
	f32 frameTimer = 0;

	DelegateHandle tickDelegateHandle = 0;
};

extern EditorLoop gEditorLoop;
