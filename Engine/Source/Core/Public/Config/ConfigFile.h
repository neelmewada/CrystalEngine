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
        
        String& GetStringValue()
        {
            return arrayValue[0];
        }
        
        const String& GetStringValue() const
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

        bool IsArrayValue() const
        {
            return isArrayValue;
        }

        void SetAsString()
        {
            isArrayValue = false;
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
        
    private:
        friend class ConfigFile;
        
    };

    class CORE_API ConfigFile : public HashMap<Name, ConfigSection>
    {
    public:
        ConfigFile();
        virtual ~ConfigFile();

        bool SectionExists(const Name& sectionName)
        {
            return this->KeyExists(sectionName);
        }
        
        void Read(const String& fileName);
        
    private:
        
        void ReadInternal(const IO::Path& configPath);
        
    };
    
} // namespace CE
