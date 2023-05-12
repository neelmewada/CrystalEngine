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

        virtual bool IsValid() = 0;

        virtual bool IsRoot() = 0;

        virtual void EnterMap(const String& identifier = "") = 0;
        virtual void ExitMap() = 0;
    };

    class CORE_API StructuredStreamFormatterException : public Exception
    {
    public:
        StructuredStreamFormatterException(const String& message) : Exception(message)
        {}
    };
    
} // namespace CE
