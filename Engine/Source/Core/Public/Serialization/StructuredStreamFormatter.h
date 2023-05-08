#pragma once

namespace CE
{
    class Stream;
    
    namespace StructuredStreamPrivate
    {
        enum class EntryType
        {
            Root,
            Field,
            Array,
            Map
        };
    }

    class CORE_API StructuredStreamFormatter
    {
    public:
        virtual ~StructuredStreamFormatter() = default;

        virtual Stream& GetUnderlyingStream() = 0;

        virtual bool CanRead() = 0;
        virtual bool CanWrite() = 0;
        
    };
    
} // namespace CE
