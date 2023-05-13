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

        virtual bool IsValid() { return false; }

        virtual Type GetType() { return Type::None; }

        virtual StructuredStreamEntry& operator[](u32 index);
        virtual StructuredStreamEntry& operator[](const String& key);
        
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
        
        StructuredStreamEntry& operator[](const String& key) override;
        
        HashMap<String, StructuredStreamEntry*> map{};
    };
    
}
