
#include "System.h"

#include "System.private.h"
CE_IMPLEMENT_MODULE_AUTORTTI(System, CE::SystemModule)

namespace CE
{
    
    
    void SystemModule::StartupModule()
    {
        AssetDatabase::Get().Initialize();
    }

    void SystemModule::ShutdownModule()
    {
        AssetDatabase::Get().Shutdown();
    }

    void SystemModule::RegisterTypes()
    {

        // Game Framework Types
        CE_REGISTER_TYPES(
            Engine,
            GameObject,
            GameComponent,
            Scene
        );
        
        // Configs
        CE_REGISTER_TYPES(
            ConfigBase,
            PluginConfig
        );

        // Register Game Components
        CE_REGISTER_GAMECOMPONENTS(
            TransformComponent
        );

        // Register Asset Types
        CE_REGISTER_ASSET_TYPE(TextureAsset, casset, jpg, jpeg, png);
    }

} // namespace CE

