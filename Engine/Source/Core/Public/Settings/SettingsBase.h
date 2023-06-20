#pragma once

namespace CE
{

    class CORE_API SettingsBase : public Object
    {
        CE_CLASS(SettingsBase, Object)
    public:
        
        SettingsBase() = default;
        
        static SettingsBase* LoadSettings(ClassType* settingsClass, String settingsName = "");

		static void SaveSettings();
        
    };

	template<typename TSettings> requires TIsBaseClassOf<SettingsBase, TSettings>::Value
	TSettings* GetSettings()
	{
		return (TSettings*)SettingsBase::LoadSettings(TSettings::Type());
	}
    
	FORCE_INLINE void SaveSettings()
	{
		SettingsBase::SaveSettings();
	}
}


CE_RTTI_CLASS(CORE_API, CE, SettingsBase,
    CE_SUPER(CE::Object),
    CE_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(
                  
    ),
    CE_FUNCTION_LIST()
)
