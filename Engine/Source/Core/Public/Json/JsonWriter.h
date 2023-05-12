#pragma once

#include "JsonWritePolicy.h"

namespace CE
{
    namespace Json
    {
        // Escapes a string to make it compatible to be used as a Json String
        static String GetJsonEscapedString(const String& string)
        {
            String escapedString = "\"";

            for (int i = 0; i < string.GetLength(); i++)
            {
                CHAR ch = string[i];

                switch (ch)
                {
                case '\"': escapedString += "\\\""; break;
                case '\\': escapedString += "\\\\"; break;
                case '\n': escapedString += "\\\n"; break;
                case '\r': escapedString += "\\\r"; break;
                case '\t': escapedString += "\\\t"; break;
                case '\b': escapedString += "\\\b"; break;
                default:
                    escapedString.Append(ch);
                    break;
                }
            }
            
            escapedString += "\"";
            return escapedString;
        }
    }

    template<class WritePolicy = JsonPrettyPrintPolicy>
    class JsonWriter
    {
    public:
        static JsonWriter Create(Stream* outputStream, int initialIndent = 0)
        {
            return JsonWriter(outputStream, initialIndent);
        }
        
        virtual ~JsonWriter() = default;

        // - Public API -

        bool WriteObjectStart()
        {
            if (!CanWriteObjectStartWithoutIdentifier())
                return false;

            WriteCommaIfNeeded();

            if (prevToken != JsonToken::None && prevToken != JsonToken::SquareOpen)
            {
                WritePolicy::WriteLineEnding(stream);
                WritePolicy::WriteLineTabs(stream, indentLevel);
            }

            WritePolicy::WriteChar(stream, '{');
            WritePolicy::WriteLineEnding(stream);
            indentLevel++;
            WritePolicy::WriteLineTabs(stream, indentLevel);
            
            valueStack.Push(JsonValueType::Object);
            prevToken = JsonToken::CurlyOpen;
            return true;
        }
        
        bool WriteObjectStart(const String& identifier)
        {
            WriteIdentifier(identifier);
            return WriteObjectStart();
        }

        void WriteObjectClose()
        {
            if (!CanWriteObjectClose())
                return;

            valueStack.Pop();
            indentLevel--;

            WritePolicy::WriteLineEnding(stream);
            WritePolicy::WriteLineTabs(stream, indentLevel);
            
            WritePolicy::WriteChar(stream, '}');
            prevToken = JsonToken::CurlyClose;
        }

        bool WriteArrayStart()
        {
            if (!CanWriteArrayStartWithoutIdentifier())
                return false;

            WriteCommaIfNeeded();

            if (prevToken != JsonToken::None)
            {
                WritePolicy::WriteLineEnding(stream);
                WritePolicy::WriteLineTabs(stream, indentLevel);
            }

            WritePolicy::WriteChar(stream, '[');
            WritePolicy::WriteLineEnding(stream);
            indentLevel++;
            WritePolicy::WriteLineTabs(stream, indentLevel);

            valueStack.Push(JsonValueType::Array);
            prevToken = JsonToken::SquareOpen;
            return true;
        }

        bool WriteArrayStart(const String& identifier)
        {
            WriteIdentifier(identifier);
            return WriteArrayStart();
        }

        void WriteArrayClose()
        {
            if (!CanWriteArrayClose())
                return;

            valueStack.Pop();
            indentLevel--;

            WritePolicy::WriteLineEnding(stream);
            WritePolicy::WriteLineTabs(stream, indentLevel);

            WritePolicy::WriteChar(stream, ']');
            prevToken = JsonToken::SquareClose;
        }

        void WriteNull()
        {
            if (!CanWriteValueWithoutIdentifier())
                return;

            WriteCommaIfNeeded();

            if (prevToken != JsonToken::Colon && prevToken != JsonToken::SquareOpen)
            {
                WritePolicy::WriteLineEnding(stream);
                WritePolicy::WriteLineTabs(stream, indentLevel);
            }

            WritePolicy::WriteString(stream, "null");

            prevToken = JsonToken::Null;
        }

        void WriteNull(const String& identifier)
        {
            WriteIdentifier(identifier);
            WriteNull();
        }

        void WriteValue(const String& value)
        {
            if (!CanWriteValueWithoutIdentifier())
                return;

            WriteCommaIfNeeded();

            if (prevToken != JsonToken::Colon && prevToken != JsonToken::SquareOpen)
            {
                WritePolicy::WriteLineEnding(stream);
                WritePolicy::WriteLineTabs(stream, indentLevel);
            }
            
            WriteStringValue(value);
            
            prevToken = JsonToken::String;
        }

        void WriteValue(const String& identifier, const String& value)
        {
            WriteIdentifier(identifier);
            WriteValue(value);
        }

        void WriteValue(const char* value)
        {
            WriteValue(String(value));
        }

        void WriteValue(const String& identifier, const char* value)
        {
            WriteValue(identifier, String(value));
        }

        void WriteValue(f64 value)
        {
            if (!CanWriteValueWithoutIdentifier())
                return;

            WriteCommaIfNeeded();

            if (prevToken != JsonToken::Colon)
            {
                WritePolicy::WriteLineEnding(stream);
                WritePolicy::WriteLineTabs(stream, indentLevel);
            }

            WriteFloatValue(value);

            prevToken = JsonToken::Number;
        }

        void WriteValue(const String& identifier, f64 value)
        {
            WriteIdentifier(identifier);
            WriteValue(value);
        }

        void WriteValue(bool boolean)
        {
            if (!CanWriteValueWithoutIdentifier())
                return;

            WriteCommaIfNeeded();

            if (prevToken != JsonToken::Colon)
            {
                WritePolicy::WriteLineEnding(stream);
                WritePolicy::WriteLineTabs(stream, indentLevel);
            }

            WriteBoolValue(boolean);
            
            prevToken = boolean ? JsonToken::True : JsonToken::False;
        }

        void WriteValue(const String& identifier, bool value)
        {
            WriteIdentifier(identifier);
            WriteValue(value);
        }

        template<typename TInt> requires TIsIntegralType<TInt>::Value
        void WriteValue(TInt value)
        {
            if (!CanWriteValueWithoutIdentifier())
                return;

            WriteCommaIfNeeded();

            if (prevToken != JsonToken::Colon)
            {
                WritePolicy::WriteLineEnding(stream);
                WritePolicy::WriteLineTabs(stream, indentLevel);
            }

            WriteIntegerValue(value);

            prevToken = JsonToken::Number;
        }

        template<typename TInt> requires TIsIntegralType<TInt>::Value
        void WriteValue(const String& identifier, TInt value)
        {
            WriteIdentifier(identifier);
            WriteValue(value);
        }

        // - Validation API -

        bool CanWriteObjectStart() const
        {
            return CanWriteObjectStartWithoutIdentifier();
        }

        bool CanWriteObjectClose() const
        {
            return !valueStack.IsEmpty() && valueStack.GetLast() == JsonValueType::Object;
        }

        bool CanWriteArrayClose() const
        {
            return !valueStack.IsEmpty() && valueStack.GetLast() == JsonValueType::Array;
        }
        
        bool WriteIdentifier(const String& identifier)
        {
            if (!valueStack.IsEmpty() && valueStack.GetLast() != JsonValueType::Object)
                return false;

            if (WriteCommaIfNeeded() || prevToken == JsonToken::SquareOpen)
            {
                WritePolicy::WriteLineEnding(stream);
                WritePolicy::WriteLineTabs(stream, indentLevel);
            }
            
            WriteStringValue(identifier);
            WritePolicy::WriteChar(stream, ':');
            WritePolicy::WriteWhiteSpace(stream, 1);

            prevToken = JsonToken::Colon;
            return true;
        }
        
    protected:

        bool WriteCommaIfNeeded()
        {
            if (prevToken == JsonToken::CurlyClose || prevToken == JsonToken::SquareClose ||
                prevToken == JsonToken::Number || prevToken == JsonToken::String || prevToken == JsonToken::True ||
                prevToken == JsonToken::False || prevToken == JsonToken::Null)
            {
                WritePolicy::WriteChar(stream, ',');
                return true;
            }
            return false;
        }

        FORCE_INLINE bool CanWriteObjectStartWithoutIdentifier() const
        {
            return prevToken == JsonToken::None || prevToken == JsonToken::SquareOpen || prevToken == JsonToken::Colon ||
                (!valueStack.IsEmpty() && valueStack.GetLast() == JsonValueType::Array);
        }

        FORCE_INLINE bool CanWriteArrayStartWithoutIdentifier() const
        {
            return prevToken == JsonToken::None || prevToken == JsonToken::SquareOpen || prevToken == JsonToken::Colon ||
                (!valueStack.IsEmpty() && valueStack.GetLast() == JsonValueType::Array);
        }

        FORCE_INLINE bool CanWriteValueWithoutIdentifier() const
        {
            return prevToken == JsonToken::SquareOpen || prevToken == JsonToken::Colon ||
                (!valueStack.IsEmpty() && valueStack.GetLast() == JsonValueType::Array);
        }

        FORCE_INLINE void WriteStringValue(const String& string)
        {
            WritePolicy::WriteString(stream, Json::GetJsonEscapedString(string));
        }

        FORCE_INLINE void WriteFloatValue(f64 value)
        {
            WritePolicy::WriteString(stream, String::Format("{}", (f32)value));
        }

        FORCE_INLINE void WriteBoolValue(b8 boolean)
        {
            WritePolicy::WriteBool(stream, boolean);
        }

        template<typename TInt> requires TIsIntegralType<TInt>::Value
        FORCE_INLINE void WriteIntegerValue(TInt value)
        {
            WritePolicy::WriteString(stream, String::Format("{}", value));
        }

        JsonWriter(Stream* stream, u32 initialIndent = 0)
            : stream(stream), indentLevel(initialIndent)
        {}

        Stream* stream = nullptr;
        u32 indentLevel = 0;
        Array<JsonValueType> valueStack{};
        JsonToken prevToken = JsonToken::None;
    };

    using PrettyJsonWriter = JsonWriter<JsonPrettyPrintPolicy>;
    using CondensedJsonWriter = JsonWriter<JsonCondensedPrintPolicy>;
    
} // namespace CE
