
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
		Cleanup();
    }

	void JsonValue::Cleanup()
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

	void JsonValue::Clear()
	{
		if (valueType == JsonValueType::String)
		{
			stringValue.~String();
		}
		else if (valueType == JsonValueType::Object)
		{
			objectValue.~JsonObject();
		}
		else if (valueType == JsonValueType::Array)
		{
			arrayValue.~JsonArray();
		}

		memset(this, 0, sizeof(JsonValue));
	}

	void JsonValue::Copy(const JsonValue& copy)
    {
		if (valueType == JsonValueType::String)
		{
			stringValue.~String();
		}
		else if (valueType == JsonValueType::Object)
		{
			objectValue.~JsonObject();
		}
		else if (valueType == JsonValueType::Array)
		{
			arrayValue.~JsonArray();
		}

		Clear();

		this->valueType = copy.valueType;
		if (copy.valueType == JsonValueType::Array)
		{
			this->arrayValue = JsonArray(copy.arrayValue);
		}
		else if (copy.valueType == JsonValueType::Object)
		{
			this->objectValue = JsonObject(copy.objectValue);
		}
		else if (copy.valueType == JsonValueType::String)
		{
			this->stringValue = copy.stringValue;
		}
		else if (copy.valueType == JsonValueType::Number)
		{
			this->numberValue = copy.numberValue;
		}
		else if (copy.valueType == JsonValueType::Boolean)
		{
			this->boolValue = copy.boolValue;
		}
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

    bool JsonSerializer::Deserialize2(Stream* stream, JValue& out)
    {
		Deserialize(nullptr);
		if (stream == nullptr)
		{
			CE_LOG(Error, All, "JSON Deserialize: Input stream is null");
			return false;
		}

		if (!stream->CanRead())
		{
			CE_LOG(Error, All, "JSON Deserialize: Input stream cannot be read!");
			return false;
		}

		out = nullptr;

		auto reader = JsonReader::Create(stream);
		JsonReadInstruction instruction = JsonReadInstruction::None;

		Array<JValue*> stack{};

		while (reader.ParseNext(instruction))
        {
            if (reader.GetError() != JsonParseError::None)
            {
                CE_LOG(Error, All, "JSON Deserialize: Failed to deserialize JSON: {}", reader.GetErrorMessage());
                return false;
            }
            
            switch (instruction)
            {
            case JsonReadInstruction::ObjectStart:
                {
                    if (out.IsNullValue()) // Root entry
                    {
						out = JValue(JObject());
                        stack.Push(&out);
                    }
                    else if (stack.NonEmpty())
                    {
                        auto objectValue = JValue(JObject());
                        if (stack.Top()->IsObjectValue())
                        {
                            stack.Top()->GetObjectValue().Add({ reader.GetIdentifier(), objectValue });
							stack.Push(&stack.Top()->GetObjectValue()[reader.GetIdentifier()]);
                        }
                        else if (stack.Top()->IsArrayValue())
                        {
                            stack.Top()->GetArrayValue().Add(objectValue);
							stack.Push(&stack.Top()->GetArrayValue().GetLast());
                        }
                        else
                        {
                            CE_LOG(Error, All, "JSON Deserialize: Failed to add object to stack!");
                            return false;
                        }
                    }
                    else
                    {
                        CE_LOG(Error, All, "JSON Deserialize: Failed to open object!");
                        return false;
                    }
                }
                break;
            case JsonReadInstruction::ObjectEnd:
                {
                    if (stack.IsEmpty() || !stack.Top()->IsObjectValue())
                    {
                        CE_LOG(Error, All, "JSON Deserialize: Failed to close object. Stack empty or invalid!");
                        return false;
                    }
                    stack.Pop();
                }
                break;
            case JsonReadInstruction::ArrayStart:
                {
                    if (out.IsNullValue()) // Root entry
                    {
						out = JValue(JArray());
                        stack.Push(&out);
                    }
                    else if (stack.NonEmpty())
                    {
                        auto arrayValue = JValue(JArray());
                        if (stack.Top()->IsObjectValue())
                        {
                            stack.Top()->GetObjectValue().Add({ reader.GetIdentifier(), arrayValue });
							stack.Push(&stack.Top()->GetObjectValue()[reader.GetIdentifier()]);
                        }
                        else if (stack.Top()->IsArrayValue())
                        {
                            stack.Top()->GetArrayValue().Add(arrayValue);
							stack.Push(&stack.Top()->GetArrayValue().GetLast());
                        }
                        else
                        {
                            CE_LOG(Error, All, "JSON Deserialize: Failed to add object to stack!");
                            return false;
                        }
                    }
                }
                break;
            case JsonReadInstruction::ArrayEnd:
                {
                    if (stack.IsEmpty() || !stack.Top()->IsArrayValue())
                    {
                        CE_LOG(Error, All, "JSON Deserialize: Failed to close array. Stack empty or invalid!");
                        return false;
                    }
                    stack.Pop();
                }
                break;
            case JsonReadInstruction::String:
                {
                    if (stack.IsEmpty() || !stack.Top()->IsContainerType())
					{
                        CE_LOG(Error, All, "JSON Deserialize: Failed to add string value. Stack empty or invalid!");
                        return false;
                    }
                    auto stringValue = JValue(reader.GetStringValue());
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
                        CE_LOG(Error, All, "JSON Deserialize: Failed to add boolean value. Stack empty or invalid!");
                        return false;
                    }
                    auto boolValue = JValue(reader.GetBoolValue());
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
                        CE_LOG(Error, All, "JSON Deserialize: Failed to add number value. Stack empty or invalid!");
                        return false;
                    }
                    auto numberValue = JValue(reader.GetNumberValue());
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
                        CE_LOG(Error, All, "JSON Deserialize: Failed to add null value. Stack empty or invalid!");
                        return false;
                    }
                    auto nullValue = JValue();
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
                CE_LOG(Error, All, "JSON Deserialize: Invalid instruction found: JsonReadInstruction::None");
                return false;
            }
        }

		return true;
    }

	bool JsonSerializer::Deserialize2(const String& json, JValue& out)
	{
		MemoryStream stream = MemoryStream(json.GetData(), json.GetLength(), Stream::Permissions::ReadOnly);
		return Deserialize2(&stream, out);
	}

} // namespace CE

