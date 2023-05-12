
#include "CoreMinimal.h"

namespace CE
{
    
    JsonStreamInputFormatter::JsonStreamInputFormatter(Stream& stream)
        : stream(stream), currentEntry({})
    {
        if (!stream.CanRead())
            throw StructuredStreamFormatterException("JsonStreamInputFormatter passed with a stream that cannot be written to!");

        // Load in Json values
        internalValue = JsonSerializer::Deserialize(&stream);
    }

    JsonStreamInputFormatter::~JsonStreamInputFormatter()
    {
        
    }

    Stream& JsonStreamInputFormatter::GetUnderlyingStream()
    {
        return stream;
    }

    bool JsonStreamInputFormatter::CanRead()
    {
        return true;
    }

    bool JsonStreamInputFormatter::CanWrite()
    {
        return false;
    }

    bool JsonStreamInputFormatter::IsValid()
    {
        return stream.CanRead();
    }

    bool JsonStreamInputFormatter::IsRoot()
    {
        currentEntry = StructuredStream::Array({});
        return currentEntry.IsRootEntry();
    }
    
}
