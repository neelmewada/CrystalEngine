
#include "Core.h"

namespace CE
{
    // Module Class
    class CoreModule : public Module
    {
    public:
        void StartupModule() override;
        void ShutdownModule() override;

        void RegisterTypes() override;

    private:
        DelegateHandle onBeforeModuleUnloadHandle{};
    };


    Package* gTransientPackage = nullptr;

    void CoreModule::StartupModule()
    {
#if PAL_TRAIT_BUILD_EDITOR
        if (gProjectPath.IsEmpty()) // Editor: gProjectPath should be set before loading Core
            gProjectPath = PlatformDirectories::GetEngineRootDir();
#else
        gProjectPath = PlatformDirectories::GetGameRootDir(); // Runtime: gProjectPath is always the install directory
#endif

        // Load Configs cache
        gConfigCache = new ConfigCache;
        gConfigCache->LoadStartupConfigs();
    
        gTransientPackage = CreateObject<Package>(nullptr, TEXT("/Engine/Transient"), OF_Transient);
        
        onBeforeModuleUnloadHandle = CoreDelegates::onBeforeModuleUnload.AddDelegateInstance(&TypeInfo::DeregisterTypesForModule);
    }

    void CoreModule::ShutdownModule()
    {
        gTransientPackage->RequestDestroy();
        gTransientPackage = nullptr;

        delete gConfigCache;
        gConfigCache = nullptr;

        gProjectPath = "";
        
        CoreDelegates::onBeforeModuleUnload.RemoveDelegateInstance(onBeforeModuleUnloadHandle);
        onBeforeModuleUnloadHandle = 0;
    }

    void CoreModule::RegisterTypes()
    {
        // Register Data types
        CE_REGISTER_TYPES(b8, s8, s16, s32, s64,
            u8, u16, u32, u64, f32, f64,
            CE::String, CE::UUID, CE::Name, CE::Array<u8>,
            CE::Vec2, CE::Vec3, CE::Vec4, CE::Vec2i, CE::Vec3i, CE::Vec4i,
            CE::Quat, CE::Matrix4x4,
			CE::ObjectMap);

        // Enums
        CE_REGISTER_TYPES(
            EventResult
        );

        // Register Objects
        CE_REGISTER_TYPES(
            Object,
            Package,
            SystemObject,
            Component,
            SystemComponent,
			Asset
        );
    }

}


CE_IMPLEMENT_MODULE(Core, CE::CoreModule)
