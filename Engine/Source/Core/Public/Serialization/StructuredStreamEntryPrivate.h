#pragma once

namespace CE
{
    class StructuredStream;
}

namespace CE::StructuredStreamPrivate
{

    struct EntryKey
    {
        EntryKey(int index) : index(index), isInteger(true)
        {}

        EntryKey(const String& key) : key(key), isInteger(false)
        {}

        EntryKey(const char* key) : key(key), isInteger(false)
        {}

        bool IsIndex() const { return isInteger; }
        bool IsKey() const { return !isInteger; }
        
        bool isInteger = false;
        int index = 0;
        String key = "";
    };

    class EntryBase
    {
    public:
        EntryBase()
        {}
        virtual ~EntryBase()
        {}

        bool IsRootEntry() const
        {
            return positionStack.IsEmpty();
        }
        
        friend class CE::StructuredStream;
        
        Array<EntryKey> positionStack{};
    };
    
}

namespace CE
{
    using StructuredStreamPosition = Array<StructuredStreamPrivate::EntryKey>;
    
}
