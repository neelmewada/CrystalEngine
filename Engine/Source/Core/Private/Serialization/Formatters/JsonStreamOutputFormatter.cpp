
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

    void JsonStreamOutputFormatter::EnterMap()
    {
        writer.WriteObjectStart();
    }

    void JsonStreamOutputFormatter::ExitMap()
    {
        writer.WriteObjectClose();
    }

    void JsonStreamOutputFormatter::EnterArray()
    {
        writer.WriteArrayStart();
    }

    void JsonStreamOutputFormatter::ExitArray()
    {
        writer.WriteArrayClose();
    }

    void JsonStreamOutputFormatter::EnterField(const String& identifier)
    {
        writer.WriteIdentifier(identifier);
    }

    void JsonStreamOutputFormatter::ExitField()
    {
        
    }

    void JsonStreamOutputFormatter::EnterStringValue(const String& value)
    {
        writer.WriteValue(value);
    }

    void JsonStreamOutputFormatter::EnterNumberValue(f64 value)
    {
        writer.WriteValue(value);
    }

    void JsonStreamOutputFormatter::EnterBoolValue(bool value)
    {
        writer.WriteValue(value);
    }

    void JsonStreamOutputFormatter::EnterNullValue()
    {
        writer.WriteNull();
    }
    
} // namespace CE

