#pragma once

#include "CoreTypes.h"

namespace CE
{

    struct CORE_API ConfigValue
    {
    public:
        ConfigValue() : arrayValue(Array<String>({""})), isArrayValue(false)
        {}
        
        ConfigValue(const String& string) : arrayValue(Array<String>({ string })), isArrayValue(false)
        {}

        ConfigValue(const Array<String>& array) : arrayValue(array), isArrayValue(true)
        {}
        
        ConfigValue(const ConfigValue& copy)
            : arrayValue(copy.arrayValue)
        {}
        
        ConfigValue operator=(const ConfigValue& copy)
        {
            return ConfigValue(copy);
        }
        
        inline operator String() const
        {
            return arrayValue[0];
        }
        
        String& GetString()
        {
            return arrayValue[0];
        }
        
        const String& GetString() const
        {
            return arrayValue[0];
        }

        Array<String>& GetArray()
        {
            return arrayValue;
        }

        const Array<String>& GetArray() const
        {
            return arrayValue;
        }

        bool IsValid() const
        {
            return arrayValue.GetSize() > 0;
        }

        bool IsArray() const
        {
            return isArrayValue;
        }

        void SetAsString()
        {
            isArrayValue = false;
        }

        void SetAsString(String value)
        {
            isArrayValue = false;
            arrayValue.Clear();
            arrayValue.Add(value);
        }

        void SetAsArray()
        {
            isArrayValue = true;
        }

        void AddValue(const String& value)
        {
            arrayValue.Add(value);
        }

        void RemoveValue(const String& value)
        {
            arrayValue.Remove(value);
        }
        
    private:
        bool isArrayValue = false;

        Array<String> arrayValue{};
    };

    typedef HashMap<String, ConfigValue> ConfigSectionMap;

    class CORE_API ConfigSection : public ConfigSectionMap
    {
    public:

        bool ValueExists(const String& configKey) const
        {
            return KeyExists(configKey);
        }
        
    private:
        friend class ConfigFile;
        
    };

    class CORE_API ConfigFile : public HashMap<Name, ConfigSection>
    {
    public:
        typedef HashMap<Name, ConfigSection> Super;
        
        ConfigFile();
        virtual ~ConfigFile();

        bool SectionExists(const Name& sectionName)
        {
            return this->KeyExists(sectionName);
        }

        void Read(const IO::Path& configPath);
        
    private:
        
    };

    class CORE_API ConfigCache
    {
    public:
        ConfigCache();
        virtual ~ConfigCache();

        String FindBaseConfigFileName(ConfigType type);
        String FindBaseConfigFileName(ConfigType type, String platform);

        void LoadStartupConfigs();
        
        void LoadConfig(const ConfigType& type);

        Array<IO::Path> GetConfigPaths(const ConfigType& type);
        
        ConfigFile* GetConfigFile(const ConfigType& type);

        void Clear();
        
    private:
        // Map: ConfigType -> ConfigFile
        HashMap<ConfigType, ConfigFile*> cache{};
    };
    
} // namespace CE
