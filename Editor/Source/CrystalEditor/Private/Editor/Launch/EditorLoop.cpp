
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
        CE::ModuleManager::Get().LoadModule("CoreMedia");
		CE::ModuleManager::Get().LoadModule("System");

		// Graphics API Selection
#if PAL_TRAIT_VULKAN_SUPPORTED
		CE::ModuleManager::Get().LoadModule("VulkanRHI");
#endif

		// Load Earliest plugins
		PluginManager::Get().Init();
		PluginManager::Get().LoadPlugins(CE::PluginLoadType::LoadEarliest);

        CE::ModuleManager::Get().LoadModule("QtComponents");
		CE::ModuleManager::Get().LoadModule("EditorCore");
		CE::ModuleManager::Get().LoadModule("EditorSystem");

		// Pre-Init plugins
		PluginManager::Get().LoadPlugins(CE::PluginLoadType::LoadOnPreInit);
	}

	void EditorLoop::PostInit()
	{
		CE::ModuleManager::Get().LoadModule("CrystalEditor");

		// Init plugins
		PluginManager::Get().LoadPlugins(CE::PluginLoadType::LoadOnInit);
		// Post-Init plugins
		PluginManager::Get().LoadPlugins(CE::PluginLoadType::LoadOnPostInit);

		// Initialize RHI, device, etc
		gDynamicRHI->Initialize();
		gDynamicRHI->PostInitialize();
	}

	void EditorLoop::PreShutdown()
	{
		CE::ModuleManager::Get().UnloadModule("CrystalEditor");

        // Shutdown RHI
        gDynamicRHI->PreShutdown();
        gDynamicRHI->Shutdown();
        
		CE::ModuleManager::Get().UnloadModule("EditorSystem");
		CE::ModuleManager::Get().UnloadModule("EditorCore");
        CE::ModuleManager::Get().UnloadModule("QtComponents");

		PluginManager::Get().UnloadAllPlugins();
	}

	void EditorLoop::Shutdown()
	{
#if PAL_TRAIT_VULKAN_SUPPORTED
		CE::ModuleManager::Get().UnloadModule("VulkanRHI");
#endif

		CE::ModuleManager::Get().UnloadModule("System");
        CE::ModuleManager::Get().UnloadModule("CoreMedia");
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
