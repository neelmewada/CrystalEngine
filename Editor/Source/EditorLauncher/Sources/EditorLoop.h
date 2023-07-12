#pragma once

using namespace CE::Widgets;

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

	void UnloadProject();

	void InitStyles();

	void AppPreInit();
	void AppInit();

	void AppPreShutdown();
	void AppShutdown();

	void LoadStartupCoreModules();
	void LoadCoreModules();

	void LoadEditorModules();
	void UnloadEditorModules();

	// Application
	CE::PlatformApplication* app = nullptr;

	// RHI
	CE::RHI::Viewport* viewport = nullptr;
	CE::RHI::GraphicsCommandList* cmdList = nullptr;

	IO::Path projectPath{};

	CWindow* rootWindow = nullptr;
};

extern EditorLoop gEditorLoop;
