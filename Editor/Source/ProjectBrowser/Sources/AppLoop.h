#pragma once

using namespace CE;
using namespace CE::Editor;

class ProjectBrowserWindow;

class AppLoop
{
public:

	void PreInit(int argc, const char** argv);
	void Init();
	void PostInit();

	void RunLoop();

	void PreShutdown();
	void Shutdown();

private:

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

	// Widgets

	ProjectBrowserWindow* projectBrowser = nullptr;
};

extern AppLoop gLoop;
