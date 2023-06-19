#pragma once

namespace CE
{
    class CORE_API SettingsBase : public Object
    {
        CE_CLASS(SettingsBase, Object)
    public:
        
        SettingsBase() = default;
        
        SettingsBase* LoadSettings(ClassType* settingsClass, String settingsName = "");
        
    };

    
    
}


CE_RTTI_CLASS(CORE_API, CE, SettingsBase,
    CE_SUPER(CE::Object),
    CE_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(
                  
    ),
    CE_FUNCTION_LIST()
)
