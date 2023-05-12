
#include "CoreMinimal.h"

namespace CE
{

    JsonStreamOutputFormatter::JsonStreamOutputFormatter(Stream& stream)
        : stream(stream), currentEntry({}), writer(PrettyJsonWriter::Create(&stream))
    {
        if (!stream.CanWrite())
            throw StructuredStreamFormatterException("JsonStreamOutputFormatter passed with a stream that cannot be written to!");

        
    }

    JsonStreamOutputFormatter::~JsonStreamOutputFormatter()
    {
    }

    Stream& JsonStreamOutputFormatter::GetUnderlyingStream()
    {
        return stream;
    }

    bool JsonStreamOutputFormatter::CanRead()
    {
        return false;
    }

    bool JsonStreamOutputFormatter::CanWrite()
    {
        return true;
    }

    bool JsonStreamOutputFormatter::IsValid()
    {
        return stream.CanWrite();
    }

    bool JsonStreamOutputFormatter::IsRoot()
    {
        return currentEntry.IsRootEntry();
    }

    void JsonStreamOutputFormatter::EnterMap(const String& identifier)
    {
        
    }
    
} // namespace CE

