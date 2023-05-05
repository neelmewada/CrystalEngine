
#include "System.h"

#include "System.private.h"
CE_IMPLEMENT_MODULE(System, CE::SystemModule)

namespace CE
{
#if AUTORTTI
    int i = 0;
#endif
    
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

        // Register Asset Types
        CE_REGISTER_ASSET_TYPE(TextureAsset, casset, jpg, jpeg, png);

        // Register Resource Classes
        CE_REGISTER_RESOURCE_CLASS(TextureAsset, Texture);
    }

} // namespace CE

