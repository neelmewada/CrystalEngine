#pragma once

using namespace CE;
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

	void LoadEngineModules();
	void UnloadEngineModules();

	void LoadEditorModules();
	void UnloadEditorModules();

	// Application
	CE::PlatformApplication* app = nullptr;

	// RHI
	CE::RHI::Viewport* viewport = nullptr;
	CE::RHI::GraphicsCommandList* cmdList = nullptr;

	IO::Path projectPath{};

	CrystalEditorWindow* editorWindow = nullptr;

	clock_t previousTime{};
};

extern EditorLoop gEditorLoop;
