
#include "CoreMinimal.h"

namespace CE
{
    
    JsonStreamInputFormatter::JsonStreamInputFormatter(Stream& stream)
        : stream(stream)
    {
        if (!stream.CanRead())
            throw StructuredStreamFormatterException("JsonStreamInputFormatter passed with a stream that cannot be written to!");

        // Load in Json values
        rootValue = JsonSerializer::Deserialize(&stream);
    }

    JsonStreamInputFormatter::~JsonStreamInputFormatter()
    {
        delete rootValue;
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
        return valueStack.GetSize() == 0;
    }

    void JsonStreamInputFormatter::EnterMap()
    {
        if (valueStack.GetSize() == 0)
        {
            valueStack.Push(rootValue);
            return;
        }
        
        if (valueStack.Top()->IsObjectValue() && !currentIdentifier.IsEmpty())
        {
            valueStack.Push(valueStack->Top()->GetObjectValue()[currentIdentifier]);
        }
        else if (valueStack.Top()->IsArrayValue())
        {
            valueStack.Push(valueStack->Top()->GetArrayValue()[currentArrayIndex]);
        }
    }

    void JsonStreamInputFormatter::ExitMap()
    {
        valueStack.Pop();
    }

    void JsonStreamInputFormatter::EnterArray()
    {
        if (valueStack.GetSize() == 0)
        {
            valueStack.Push(rootValue);
            return;
        }
        
        if (valueStack.Top()->IsObjectValue() && !currentIdentifier.IsEmpty())
        {
            valueStack.Push(valueStack->Top()->GetObjectValue()[currentIdentifier]);
        }
        else if (valueStack.Top()->IsArrayValue())
        {
            valueStack.Push(valueStack->Top()->GetArrayValue()[currentArrayIndex]);
        }
    }

    void JsonStreamInputFormatter::ExitArray()
    {
        valueStack.Pop();
    }

    void JsonStreamInputFormatter::EnterField(const CE::String& identifier)
    {
        
    }

    void JsonStreamInputFormatter::ExitField()
    {
        
    }

    void JsonStreamInputFormatter::EnterStringValue(const CE::String& value)
    {
        
    }

    void JsonStreamInputFormatter::EnterNumberValue(f64 value)
    {
        
    }

    void JsonStreamInputFormatter::EnterBoolValue(bool value)
    {
        
    }

    void JsonStreamInputFormatter::EnterNullValue()
    {
        
    }

    
}
