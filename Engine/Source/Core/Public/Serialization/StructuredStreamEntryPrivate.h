#pragma once

namespace CE::StructuredStreamPrivate
{
    class StructuredStream;

    struct EntryKey
    {
        EntryKey(int index) : index(index), isInteger(true)
        {}

        EntryKey(String key) : key(key), isInteger(false)
        {}
        
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

    protected:
        friend class StructuredStream;
        
        Array<EntryKey> positionStack{};
    };
    
}
