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

    class EntryPosition
    {
    public:

    protected:
        Array<EntryKey> positionStack{};

        friend class StructuredStream;
    };

    class EntryBase : protected EntryPosition
    {
    public:
        EntryBase(StructuredStream& stream) : stream(stream)
        {}

    protected:
        StructuredStream& stream;
    };
    
}
