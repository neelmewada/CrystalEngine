
#include "Launch/EditorLoop.h"

#include "CoreMinimal.h"

#include "EditorCore.h"
#include "EditorSystem.h"
#include "CrystalEditor.h"

namespace CE::Editor
{
    
    int EditorLoop::RunLoop(int argc, char** argv)
    {
		CE::Logger::Initialize();
		CE::ModuleManager::Get().LoadModule("Core");
		CE::ModuleManager::Get().LoadModule("System");

		// Load Earliest plugins
		PluginManager::Get().Init();
		PluginManager::Get().LoadPlugins(CE::PluginLoadType::LoadEarliest);

		CE::ModuleManager::Get().LoadModule("EditorCore");
		CE::ModuleManager::Get().LoadModule("EditorSystem");

		CrystalEditorApplication app{ argc, argv };

		// Pre-Init plugins
		PluginManager::Get().LoadPlugins(CE::PluginLoadType::LoadOnPreInit);

		app.Initialize();

		// Init plugins
		PluginManager::Get().LoadPlugins(CE::PluginLoadType::LoadOnInit);
		// Post-Init plugins
		PluginManager::Get().LoadPlugins(CE::PluginLoadType::LoadOnPostInit);

		auto value = app.exec();

		CE::ModuleManager::Get().UnloadModule("EditorSystem");
		CE::ModuleManager::Get().UnloadModule("EditorCore");

		PluginManager::Get().UnloadAllPlugins();

		CE::ModuleManager::Get().UnloadModule("System");
		CE::ModuleManager::Get().UnloadModule("Core");
		CE::Logger::Shutdown();
        
		return value;
    }

} // namespace CE::Editor
