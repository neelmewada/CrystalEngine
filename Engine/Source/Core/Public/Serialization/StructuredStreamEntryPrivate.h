#pragma once

namespace CE::StructuredStreamPrivate
{
    class StructuredStream;

    class EntryPosition
    {
    public:
        EntryPosition(s32 depth, u32 id) : depth(depth), id(id)
        {}
        
        s32 depth;
        u32 id;

        friend class StructuredStream;
    };

    class EntryBase : protected EntryPosition
    {
    public:
        EntryBase(StructuredStream& stream, s32 depth, u32 id)
            : EntryPosition(depth, id), stream(stream)
        {}

    protected:
        StructuredStream& stream;
    };
    
}
