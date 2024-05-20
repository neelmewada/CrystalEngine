#pragma once

namespace CE
{

    class CORE_API Settings : public Object
    {
        CE_CLASS(Settings, Object)
    public:
        
        Settings() = default;
        
        static Settings* LoadSettings(ClassType* settingsClass, String settingsName = "");

        static void SaveSettings();

#if PAL_TRAIT_BUILD_EDITOR
        static void SaveSettings(const IO::Path& customPath);
#endif

		static Array<ClassType*> GetSettingsClassesWithCategory(const String& settingsCategory);

		FORCE_INLINE static const Array<ClassType*>& GetAllSettingsClasses() { return settingsClasses; }
        
	private:

		static void OnClassRegistered(ClassType* classType);
		static void OnClassDeregistered(ClassType* classType);

		static Array<ClassType*> settingsClasses;

        friend class CoreModule;
    };

	template<typename TSettings> requires TIsBaseClassOf<Settings, TSettings>::Value
	TSettings* GetSettings()
	{
		return (TSettings*)Settings::LoadSettings(TSettings::Type());
	}
    
	FORCE_INLINE void SaveSettings()
	{
		Settings::SaveSettings();
	}

#if PAL_TRAIT_BUILD_EDITOR
    FORCE_INLINE void SaveSettings(const IO::Path& outPackagePath)
	{
        Settings::SaveSettings(outPackagePath);
	}
#endif
}


CE_RTTI_CLASS(CORE_API, CE, Settings,
    CE_SUPER(CE::Object),
    CE_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(
                  
    ),
    CE_FUNCTION_LIST()
)
