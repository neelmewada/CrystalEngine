#pragma once

#include "JsonReader.h"
#include "JsonWriter.h"

namespace CE
{
    class CORE_API JsonSerializer
    {
    public:
        CE_STATIC_CLASS(JsonSerializer);

        static JsonValue* Deserialize(Stream* stream);

        template<typename WritePolicy = JsonPrettyPrintPolicy>
        static void Serialize(Stream* stream, const JsonValue* json)
        {
            if (stream == nullptr || !stream->CanWrite())
                return;
            if (json == nullptr || !json->IsContainerType())
                return;

            auto writer = JsonWriter<WritePolicy>::Create(stream);
            bool isObject = json->IsObjectValue();

            if (json->IsObjectValue())
            {
                writer.WriteObjectStart();
                {
                    SerializeInternal(writer, json);
                }
                writer.WriteObjectClose();
            }
            else if (json->IsArrayValue())
            {
                writer.WriteArrayStart();
                {
                    SerializeInternal(writer, json);
                }
                writer.WriteArrayClose();
            }

            stream->Write('\0');
        }
        
    private:

        template<typename WritePolicy = JsonPrettyPrintPolicy>
        static void SerializeInternal(JsonWriter<WritePolicy>& writer, const JsonValue* parent)
        {
            if (parent == nullptr || !parent->IsContainerType())
                return;

            if (parent->IsObjectValue())
            {
                for (const auto& [identifier, json] : parent->objectValue)
                {
                    if (json->IsObjectValue())
                    {
                        writer.WriteObjectStart(identifier);
                        {
                            SerializeInternal(writer, json);
                        }
                        writer.WriteObjectClose();
                    }
                    else if (json->IsArrayValue())
                    {
                        writer.WriteArrayStart(identifier);
                        {
                            SerializeInternal(writer, json);
                        }
                        writer.WriteArrayClose();
                    }
                    else if (json->IsStringValue())
                    {
                        writer.WriteValue(identifier, json->stringValue);
                    }
                    else if (json->IsBoolValue())
                    {
                        writer.WriteValue(identifier, json->boolValue);
                    }
                    else if (json->IsNumberValue())
                    {
                        writer.WriteValue(identifier, json->numberValue);
                    }
                    else if (json->IsNullValue())
                    {
                        writer.WriteNull(identifier);
                    }
                }
            }
            else if (parent->IsArrayValue())
            {
                for (const auto& json : parent->arrayValue)
                {
                    if (json->IsObjectValue())
                    {
                        writer.WriteObjectStart();
                        {
                            SerializeInternal(writer, json);
                        }
                        writer.WriteObjectClose();
                    }
                    else if (json->IsArrayValue())
                    {
                        writer.WriteArrayStart();
                        {
                            SerializeInternal(writer, json);
                        }
                        writer.WriteArrayClose();
                    }
                    else if (json->IsStringValue())
                    {
                        writer.WriteValue(json->stringValue);
                    }
                    else if (json->IsBoolValue())
                    {
                        writer.WriteValue(json->boolValue);
                    }
                    else if (json->IsNumberValue())
                    {
                        writer.WriteValue(json->numberValue);
                    }
                    else if (json->IsNullValue())
                    {
                        writer.WriteNull();
                    }
                }
            }
        }
        
    };
    
} // namespace CE
