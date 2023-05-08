
#include "CoreMinimal.h"

namespace CE
{
    JsonValue::JsonValue()
    {
        Clear();
        valueType = JsonValueType::Null;
    }

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

    JsonValue::JsonValue(f64 numberValue)
        : numberValue(numberValue)
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

    JsonValue::~JsonValue()
    {
        if (IsObjectValue())
        {
            for (auto [_, object] : objectValue)
            {
                delete object;
            }
            objectValue.Clear();
        }
        else if (IsArrayValue())
        {
            for (auto object : arrayValue)
            {
                delete object;
            }
            arrayValue.Clear();
        }
        
        Clear();
    }

    /* ****************************************
     *  JsonSerializer
     */

    JsonValue* JsonSerializer::Deserialize(Stream* stream)
    {
        if (stream == nullptr)
        {
            CE_LOG(Error, All, "JSON Deserialize: Input stream is null");
            return nullptr;
        }

        if (!stream->CanRead())
        {
            CE_LOG(Error, All, "JSON Deserialize: Input stream cannot be read!");
            return nullptr;
        }

        JsonValue* rootValue = nullptr;
        Array<JsonValue*> stack{};

        auto reader = JsonReader::Create(stream);
        JsonReadInstruction instruction = JsonReadInstruction::None;

        while (reader.ParseNext(instruction))
        {
            if (reader.GetError() != JsonParseError::None)
            {
                CE_LOG(Error, All, "JSON Deserialize: Failed to deserialize JSON: {}", reader.GetErrorMessage());
                delete rootValue;
                return nullptr;
            }
            
            switch (instruction)
            {
            case JsonReadInstruction::ObjectStart:
                {
                    if (rootValue == nullptr) // Root entry
                        {
                        rootValue = new JsonValue(JsonObject());
                        stack.Push(rootValue);
                        }
                    else if (stack.NonEmpty())
                    {
                        auto objectValue = new JsonValue(JsonObject());
                        if (stack.Top()->IsObjectValue())
                        {
                            stack.Top()->GetObjectValue().Add({ reader.GetIdentifier(), objectValue });
                        }
                        else if (stack.Top()->IsArrayValue())
                        {
                            stack.Top()->GetArrayValue().Add(objectValue);
                        }
                        else
                        {
                            delete objectValue;
                            delete rootValue;
                            CE_LOG(Error, All, "JSON Deserialize: Failed to add object to stack!");
                            return nullptr;
                        }
                        stack.Push(objectValue);
                    }
                    else
                    {
                        delete rootValue;
                        CE_LOG(Error, All, "JSON Deserialize: Failed to open object!");
                        return nullptr;
                    }
                }
                break;
            case JsonReadInstruction::ObjectEnd:
                {
                    if (stack.IsEmpty() || !stack.Top()->IsObjectValue())
                    {
                        delete rootValue;
                        CE_LOG(Error, All, "JSON Deserialize: Failed to close object. Stack empty or invalid!");
                        return nullptr;
                    }
                    stack.Pop();
                }
                break;
            case JsonReadInstruction::ArrayStart:
                {
                    if (rootValue == nullptr) // Root entry
                    {
                        rootValue = new JsonValue(JsonArray());
                        stack.Push(rootValue);
                    }
                    else if (stack.NonEmpty())
                    {
                        auto arrayValue = new JsonValue(JsonArray());
                        if (stack.Top()->IsObjectValue())
                        {
                            stack.Top()->GetObjectValue().Add({ reader.GetIdentifier(), arrayValue });
                        }
                        else if (stack.Top()->IsArrayValue())
                        {
                            stack.Top()->GetArrayValue().Add(arrayValue);
                        }
                        else
                        {
                            delete arrayValue;
                            delete rootValue;
                            CE_LOG(Error, All, "JSON Deserialize: Failed to add object to stack!");
                            return nullptr;
                        }
                        stack.Push(arrayValue);
                    }
                }
                break;
            case JsonReadInstruction::ArrayEnd:
                {
                    if (stack.IsEmpty() || !stack.Top()->IsArrayValue())
                    {
                        delete rootValue;
                        CE_LOG(Error, All, "JSON Deserialize: Failed to close array. Stack empty or invalid!");
                        return nullptr;
                    }
                    stack.Pop();
                }
                break;
            case JsonReadInstruction::String:
                {
                    if (stack.IsEmpty() || !stack.Top()->IsContainerType())
                    {
                        delete rootValue;
                        CE_LOG(Error, All, "JSON Deserialize: Failed to add string value. Stack empty or invalid!");
                        return nullptr;
                    }
                    auto stringValue = new JsonValue(reader.GetStringValue());
                    if (stack.Top()->IsObjectValue())
                    {
                        stack.Top()->GetObjectValue().Add({ reader.GetIdentifier(), stringValue });
                    }
                    else if (stack.Top()->IsArrayValue())
                    {
                        stack.Top()->GetArrayValue().Add(stringValue);
                    }
                }
                break;
            case JsonReadInstruction::Boolean:
                {
                    if (stack.IsEmpty() || !stack.Top()->IsContainerType())
                    {
                        delete rootValue;
                        CE_LOG(Error, All, "JSON Deserialize: Failed to add boolean value. Stack empty or invalid!");
                        return nullptr;
                    }
                    auto boolValue = new JsonValue(reader.GetBoolValue());
                    if (stack.Top()->IsObjectValue())
                    {
                        stack.Top()->GetObjectValue().Add({ reader.GetIdentifier(), boolValue });
                    }
                    else if (stack.Top()->IsArrayValue())
                    {
                        stack.Top()->GetArrayValue().Add(boolValue);
                    }
                }
                break;
            case JsonReadInstruction::Number:
                {
                    if (stack.IsEmpty() || !stack.Top()->IsContainerType())
                    {
                        delete rootValue;
                        CE_LOG(Error, All, "JSON Deserialize: Failed to add number value. Stack empty or invalid!");
                        return nullptr;
                    }
                    auto numberValue = new JsonValue(reader.GetNumberValue());
                    if (stack.Top()->IsObjectValue())
                    {
                        stack.Top()->GetObjectValue().Add({ reader.GetIdentifier(), numberValue });
                    }
                    else if (stack.Top()->IsArrayValue())
                    {
                        stack.Top()->GetArrayValue().Add(numberValue);
                    }
                }
                break;
            case JsonReadInstruction::Null:
                {
                    if (stack.IsEmpty() || !stack.Top()->IsContainerType())
                    {
                        delete rootValue;
                        CE_LOG(Error, All, "JSON Deserialize: Failed to add null value. Stack empty or invalid!");
                        return nullptr;
                    }
                    auto nullValue = new JsonValue();
                    if (stack.Top()->IsObjectValue())
                    {
                        stack.Top()->GetObjectValue().Add({ reader.GetIdentifier(), nullValue });
                    }
                    else if (stack.Top()->IsArrayValue())
                    {
                        stack.Top()->GetArrayValue().Add(nullValue);
                    }
                }
                break;
            case JsonReadInstruction::None:
                delete rootValue;
                CE_LOG(Error, All, "JSON Deserialize: Invalid instruction found: JsonReadInstruction::None");
                return nullptr;
            }
        }

        return rootValue;
    }

} // namespace CE

