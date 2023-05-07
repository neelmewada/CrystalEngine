
#include "CoreMinimal.h"

namespace CE
{
    
    JsonValue::JsonValue(const String& string)
        : stringValue(string)
        , valueType(JsonValueType::String)
    {
        
    }

    JsonValue::JsonValue(bool boolValue)
        : boolValue(boolValue)
        , valueType(JsonValueType::Boolean)
    {
        
    }

    JsonValue::JsonValue(u64 unsignedValue)
        : unsignedValue(unsignedValue)
        , valueType(JsonValueType::Number)
    {
        
    }

    JsonValue::JsonValue(s64 signedValue)
       : signedValue(signedValue)
       , valueType(JsonValueType::Number)
    {
        
    }

    JsonValue::JsonValue(f32 floatValue)
        : floatValue(floatValue)
        , valueType(JsonValueType::Number) 
    {
        
    }

    JsonValue::JsonValue(JsonObject jsonObject)
        : objectValue(jsonObject)
        , valueType(JsonValueType::Object)
    {
        
    }

    JsonValue::JsonValue(JsonArray jsonArray)
        : arrayValue(jsonArray)
        , valueType(JsonValueType::Array)
    {
        
    }
    
} // namespace CE

