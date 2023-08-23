
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


    //Package* gTransientPackage = nullptr;
    Package* gSettingsPackage = nullptr;
	ResourceManager* gResourceManager = nullptr;

    void CoreModule::StartupModule()
    {
#if PAL_TRAIT_BUILD_EDITOR
        if (gProjectPath.IsEmpty()) // Editor: gProjectPath should be set before loading Core
            gProjectPath = PlatformDirectories::GetEngineRootDir();

		// Load Configs cache (not needed at Runtime)
		gConfigCache = new ConfigCache();
		gConfigCache->LoadStartupConfigs();
#else
        gProjectPath = PlatformDirectories::GetGameRootDir(); // Runtime: gProjectPath is always the install directory
#endif
        
        onBeforeModuleUnloadHandle = CoreDelegates::onBeforeModuleUnload.AddDelegateInstance(&TypeInfo::DeregisterTypesForModule);
    }

    void CoreModule::ShutdownModule()
    {
		if (gResourceManager != nullptr)
		{
			gResourceManager->RequestDestroy();
			gResourceManager = nullptr;
		}

		if (gSettingsPackage != nullptr)
		{
			gSettingsPackage->RequestDestroy();
			gSettingsPackage = nullptr;
		}

		if (gConfigCache != nullptr)
		{
			delete gConfigCache;
			gConfigCache = nullptr;
		}

        gProjectPath = "";
        
        CoreDelegates::onBeforeModuleUnload.RemoveDelegateInstance(onBeforeModuleUnloadHandle);
        onBeforeModuleUnloadHandle = 0;
    }

    void CoreModule::RegisterTypes()
    {
        // Register Data types
        CE_REGISTER_TYPES(b8, s8, s16, s32, s64,
            u8, u16, u32, u64, f32, f64,
            CE::String, CE::UUID, CE::UUID32, CE::Name, CE::Array<u8>,
            CE::Vec2, CE::Vec3, CE::Vec4, CE::Vec2i, CE::Vec3i, CE::Vec4i,
            CE::Quat, CE::Matrix4x4,
			CE::Color, CE::Gradient,
			CE::ObjectMap,
			CE::BinaryBlob,
			CE::SubClassType<Object>);

        // Enums
        CE_REGISTER_TYPES(
			IO::FileAction,
            EventResult
        );

        // Register Objects
        CE_REGISTER_TYPES(
            Object,
            Package,
            SettingsBase,
            ProjectSettings,
            SystemObject,
            Component,
            SystemComponent,
			Asset,
			ResourceManager,
			Resource
        );
    }

}


CE_IMPLEMENT_MODULE(Core, CE::CoreModule)
