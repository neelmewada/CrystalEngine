#pragma once

#include "StructuredStreamEntryPrivate.h"

namespace CE
{

    class CORE_API StructuredStreamEntry : public StructuredStreamPrivate::EntryBase
    {
    public:
        StructuredStreamEntry(const String& identifier = "") : identifier(identifier)
        {}
        
        virtual bool IsField() { return false; }
        virtual bool IsArray() { return false; }
        virtual bool IsMap() { return false; }

        virtual bool HasIdentifier()
        {
            return !identifier.IsEmpty();
        }

        virtual String GetIdentifier() const
        {
            return identifier;
        }

    protected:
        String identifier;
    };

    class CORE_API StructuredStreamField : public StructuredStreamEntry
    {
    public:
        StructuredStreamField()
            : isNull(true)
        {}

        bool IsField() override
        {
            return true;
        }

    protected:
        void Clear()
        {
            memset(this, 0, sizeof(*this));
        }

        void Copy(const StructuredStreamField& copy)
        {
            Clear();
            this->identifier = copy.identifier;
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
        StructuredStreamArray(const String& identifier = "") : StructuredStreamEntry(identifier)
        {}
        
        bool IsArray() override
        {
            return true;
        }
    };

    class CORE_API StructuredStreamMap : public StructuredStreamEntry
    {
    public:
        StructuredStreamMap(const String& identifier = "") : StructuredStreamEntry(identifier)
        {}

        bool IsMap() override
        {
            return true;
        }

    };
    
}
