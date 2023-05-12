
#include "CoreMinimal.h"

namespace CE
{

    JsonStreamOutputFormatter::JsonStreamOutputFormatter(Stream& stream)
        : stream(stream), writer(PrettyJsonWriter::Create(&stream))
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
        return true;
    }

    void JsonStreamOutputFormatter::EnterMap(const String& identifier)
    {
        if (identifier.IsEmpty())
            writer.WriteObjectStart();
        else
            writer.WriteObjectStart(identifier);
    }

    void JsonStreamOutputFormatter::ExitMap()
    {
        writer.WriteObjectClose();
    }
    
} // namespace CE

