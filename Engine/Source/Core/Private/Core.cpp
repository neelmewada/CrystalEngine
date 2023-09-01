
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
        DelegateHandle onBeforeModuleUnloadHandle = 0;
		DelegateHandle settingsBaseOnClassRegistered = 0;
		DelegateHandle settingsBaseOnClassDeregistered = 0;
    };


    //Package* gTransientPackage = nullptr;
    Package* gSettingsPackage = nullptr;
	ResourceManager* gResourceManager = nullptr;

    void CoreModule::StartupModule()
    {
		gConfigCache = new ConfigCache();

#if PAL_TRAIT_BUILD_EDITOR
        if (gProjectPath.IsEmpty()) // Editor: gProjectPath should be set before loading Core
            gProjectPath = PlatformDirectories::GetEngineRootDir();

		// Load .ini configs into cache (not needed at Runtime)
		gConfigCache->LoadStartupConfigs();
#else
        gProjectPath = PlatformDirectories::GetGameRootDir(); // Runtime: gProjectPath is always the install directory
#endif
        
        onBeforeModuleUnloadHandle = CoreDelegates::onBeforeModuleUnload.AddDelegateInstance(&TypeInfo::DeregisterTypesForModule);
		settingsBaseOnClassRegistered = CoreObjectDelegates::onClassRegistered.AddDelegateInstance(&SettingsBase::OnClassRegistered);
		settingsBaseOnClassDeregistered = CoreObjectDelegates::onClassDeregistered.AddDelegateInstance(&SettingsBase::OnClassDeregistered);
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
        
		CoreObjectDelegates::onClassDeregistered.RemoveDelegateInstance(settingsBaseOnClassDeregistered);
		settingsBaseOnClassDeregistered = 0;
		CoreObjectDelegates::onClassRegistered.RemoveDelegateInstance(settingsBaseOnClassRegistered);
		settingsBaseOnClassRegistered = 0;
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
            SystemObject,
            Component,
            SystemComponent,
			Asset,
			ResourceManager,
			Resource,
			SettingsBase
        );

		// Register Settings
		CE_REGISTER_TYPES(
			ProjectSettings
		);
    }

}


CE_IMPLEMENT_MODULE(Core, CE::CoreModule)
