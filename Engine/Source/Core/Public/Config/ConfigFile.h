#pragma once

#include "CoreTypes.h"

namespace CE
{

    struct CORE_API ConfigValue
    {
    public:
        ConfigValue() : stringValue("")
        {}
        
        ConfigValue(const String& string) : stringValue(string)
        {}
        
        ConfigValue(const ConfigValue& copy)
            : stringValue(copy.stringValue)
        {}
        
        ConfigValue operator=(const ConfigValue& copy)
        {
            return ConfigValue(copy);
        }
        
        inline operator String() const
        {
            return stringValue;
        }
        
        String& GetStringValue()
        {
            return stringValue;
        }
        
        const String& GetStringValue() const
        {
            return stringValue;
        }
        
    private:
        String stringValue;
    };

    typedef HashMap<String, ConfigValue> ConfigSectionMap;

    class CORE_API ConfigSection : public ConfigSectionMap
    {
    public:
        
    private:
        friend class ConfigFile;
        
    };

    class CORE_API ConfigFile : public HashMap<Name, ConfigSection>
    {
    public:
        ConfigFile();
        virtual ~ConfigFile();
        
        void Read(const String& fileName);
        
    private:
        
    };
    
} // namespace CE
