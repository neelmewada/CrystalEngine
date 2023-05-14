#pragma once

#include "StructuredStreamEntryPrivate.h"

namespace CE
{
    class StructuredStreamFormatter;

    class CORE_API StructuredStreamEntry : public StructuredStreamPrivate::EntryBase
    {
    public:
        enum class Type
        {
            None,
            Field,
            Array,
            Map,
        };
        
        StructuredStreamEntry()
        {}
        
        virtual bool IsField() { return false; }
        virtual bool IsArray() { return false; }
        virtual bool IsMap() { return false; }
        virtual bool IsString() { return false; }
        virtual bool IsNumber() { return false; }
        virtual bool IsBool() { return false; }
        virtual bool IsNull() { return false; }

        virtual bool IsValid() { return false; }

        virtual String GetStringValue() { return ""; }
        virtual bool GetBoolValue() { return false; }
        virtual f64 GetNumberValue() { return 0; }

        virtual Type GetType() { return Type::None; }

        virtual StructuredStreamEntry& operator[](u32 index);
        virtual StructuredStreamEntry& operator[](const String& key);

        virtual bool IndexExists(u32 index)
        {
            return false;
        }

        virtual bool KeyExists(const String& key)
        {
            return false;
        }

        virtual u32 GetArraySize() { return 0; }
        
        StructuredStreamFormatter* formatter = nullptr;
        String identifier = "";
    };

    class CORE_API StructuredStreamField : public StructuredStreamEntry
    {
    public:
        StructuredStreamField()
            : isNull(true)
        {}

        StructuredStreamField(const String& stringValue)
            : stringValue(stringValue), isString(true)
        {}

        StructuredStreamField(const char* stringValue)
            : stringValue(stringValue), isString(true)
        {}

        StructuredStreamField(f64 numberValue)
            : numberValue(numberValue), isNumber(true)
        {}

        StructuredStreamField(s32 numberValue)
            : numberValue(numberValue), isNumber(true)
        {}

        StructuredStreamField(bool boolValue)
            : boolValue(boolValue), isBool(true)
        {}
        
        ~StructuredStreamField()
        {}

        bool IsField() override
        {
            return true;
        }

        bool IsValid() override
        {
            return true;
        }

        Type GetType() override
        {
            return Type::Field;
        }

        virtual bool IsString() override { return isString; }
        virtual bool IsNumber() override { return isNumber; }
        virtual bool IsBool() override { return isBool; }
        virtual bool IsNull() override { return isNull; }

        virtual String GetStringValue() { return stringValue; }
        virtual bool GetBoolValue() { return boolValue; }
        virtual f64 GetNumberValue() { return numberValue; }

    protected:
        void Clear()
        {
            memset((void*)&stringValue, 0, sizeof(String) + 4 * sizeof(bool));
        }

        void Copy(const StructuredStreamField& copy)
        {
            if (copy.isString)
                this->stringValue = copy.stringValue;
            else if (copy.isBool)
                this->boolValue = copy.boolValue;
            else if (copy.isNumber)
                this->numberValue = copy.numberValue;
            else if (copy.isNull)
                this->isNull = true;
        }

        union
        {
            String stringValue{};
            f64 numberValue;
            bool boolValue;
        };

        bool isNull = false;
        bool isString = false;
        bool isBool = false;
        bool isNumber = false;
    };

    class CORE_API StructuredStreamArray : public StructuredStreamEntry
    {
    public:
        StructuredStreamArray()
        {}

        ~StructuredStreamArray() override;
        
        bool IsArray() override
        {
            return true;
        }

        bool IsValid() override
        {
            return true;
        }

        Type GetType() override
        {
            return Type::Array;
        }

        bool IndexExists(u32 index) override
        {
            return 0 <= index && index < array.GetSize();
        }

        u32 GetArraySize() override
        {
            return array.GetSize(); 
        }

        StructuredStreamEntry& operator[](u32 index) override;
        
        Array<StructuredStreamEntry*> array{};
    };

    class CORE_API StructuredStreamMap : public StructuredStreamEntry
    {
    public:
        StructuredStreamMap()
        {}

        ~StructuredStreamMap() override;

        bool IsMap() override
        {
            return true;
        }

        bool IsValid() override
        {
            return true;
        }

        Type GetType() override
        {
            return Type::Map;
        }

        bool KeyExists(const String& key) override
        {
            return map.KeyExists(key);
        }
        
        StructuredStreamEntry& operator[](const String& key) override;
        
        HashMap<String, StructuredStreamEntry*> map{};
    };
    
}
