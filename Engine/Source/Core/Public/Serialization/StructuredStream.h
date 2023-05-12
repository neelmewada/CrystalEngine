#pragma once

#include "StructuredStreamEntries.h"
#include "StructuredStreamFormatter.h"

namespace CE
{

    class CORE_API StructuredStream
    {
    public:
        enum StructuredStreamInstruction
        {
            Null,
            BeginMap,
            EndMap,
            BeginArray,
            EndArray,
            Key,
            Value
        };
        
        using Entry = StructuredStreamEntry;
        using Array = StructuredStreamArray;
        
        StructuredStream(StructuredStreamFormatter& formatter);

        bool CanRead();
        bool CanWrite();

        bool IsOpen();

        Stream& GetUnderlyingStream();

        virtual StructuredStream& operator<<(StructuredStreamInstruction writeInstruction);
        virtual StructuredStream& operator<<(const String& stringValue);
        virtual StructuredStream& operator<<(const char* cString)
        {
            return *this << String(cString);
        }
        virtual StructuredStream& operator<<(f64 numberValue);
        virtual StructuredStream& operator<<(s32 numberValue);
        virtual StructuredStream& operator<<(s64 numberValue);
        virtual StructuredStream& operator<<(bool boolValue);

    private:
        StructuredStreamFormatter& formatter;
        
        StructuredStreamInstruction pendingInstruction = Null;
    };

    // Exceptions

    class CORE_API StructuredStreamException : public Exception
    {
    public:
        StructuredStreamException(const String& message) : Exception(message)
        {}
    };
    
} // namespace CE
