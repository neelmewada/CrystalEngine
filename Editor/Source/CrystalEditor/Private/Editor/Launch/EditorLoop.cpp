
#include "Launch/EditorLoop.h"

#include "CoreMinimal.h"
#include "System.h"

#include "EditorCore.h"
#include "EditorSystem.h"
#include "CrystalEditor.h"

namespace CE::Editor
{
	void EditorLoop::PreInit()
	{
		CE::Logger::Initialize();
		CE::ModuleManager::Get().LoadModule("Core");
		CE::ModuleManager::Get().LoadModule("System");

		// Load Earliest plugins
		PluginManager::Get().Init();
		PluginManager::Get().LoadPlugins(CE::PluginLoadType::LoadEarliest);

		CE::ModuleManager::Get().LoadModule("EditorCore");
		CE::ModuleManager::Get().LoadModule("EditorSystem");

		// Pre-Init plugins
		PluginManager::Get().LoadPlugins(CE::PluginLoadType::LoadOnPreInit);
	}

	void EditorLoop::PostInit()
	{
		// Init plugins
		PluginManager::Get().LoadPlugins(CE::PluginLoadType::LoadOnInit);
		// Post-Init plugins
		PluginManager::Get().LoadPlugins(CE::PluginLoadType::LoadOnPostInit);

		// Initialize RHI
		MBUS_EVENT(RHIBus, Initialize);
		MBUS_EVENT(RHIBus, PostInitialize);
	}

	void EditorLoop::PreShutdown()
	{
		CE::ModuleManager::Get().UnloadModule("EditorSystem");
		CE::ModuleManager::Get().UnloadModule("EditorCore");

		// Shutdown RHI
		MBUS_EVENT(RHIBus, PreShutdown);
		MBUS_EVENT(RHIBus, Shutdown);

		PluginManager::Get().UnloadAllPlugins();
	}

	void EditorLoop::Shutdown()
	{
		CE::ModuleManager::Get().UnloadModule("System");
		CE::ModuleManager::Get().UnloadModule("Core");
		CE::Logger::Shutdown();
	}
    
    int EditorLoop::RunLoop(int argc, char** argv)
    {
		PreInit();

		CrystalEditorApplication app{ argc, argv };

		app.Initialize();

		PostInit();

		auto value = app.exec();

		PreShutdown();

		Shutdown();
        
		return value;
    }

} // namespace CE::Editor
