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

        virtual void EnterMap() = 0;
        virtual void ExitMap() = 0;
        
        virtual void EnterArray() = 0;
        virtual void ExitArray() = 0;
        
        virtual void EnterField(const String& identifier) = 0;
        virtual void ExitField() = 0;
        
        virtual void EnterStringValue(const String& value) = 0;
        virtual void EnterNumberValue(f64 value) = 0;
        virtual void EnterBoolValue(bool value) = 0;
        virtual void EnterNullValue() = 0;
    };

    class CORE_API StructuredStreamFormatterException : public Exception
    {
    public:
        StructuredStreamFormatterException(const String& message) : Exception(message)
        {}
    };
    
} // namespace CE
