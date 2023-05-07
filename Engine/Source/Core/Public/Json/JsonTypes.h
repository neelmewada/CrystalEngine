#pragma once


namespace CE
{

    enum class JsonValueType
    {
        None,
        Null,
        Object,
        Array,
        Boolean,
        String,
        Number
    };

    enum class JsonToken
    {
        None,
        Identifier,
        CurlyOpen,
        CurlyClose,
        SquareOpen,
        SquareClose,
        Comma,
        Colon,

        // Value tokens
        Number,
        String,
        True,
        False,
        Null
    };

    enum class JsonReadInstruction
    {
        None,
        ObjectStart,
        ObjectEnd,
        ArrayStart,
        ArrayEnd,

        Number,
        String,
        True,
        False,
        Null
    };

    class JsonValue;
    typedef HashMap<String, JsonValue> JsonObject;
    typedef Array<JsonValue> JsonArray;

    class CORE_API JsonValue
    {
    public:
        explicit JsonValue(const String& string);
        explicit JsonValue(bool boolValue);
        explicit JsonValue(u64 unsignedValue);
        explicit JsonValue(s64 signedValue);
        explicit JsonValue(f32 floatValue);
        explicit JsonValue(JsonObject jsonObject);
        explicit JsonValue(JsonArray jsonArray);

        JsonValue(const JsonValue& copy)
        {
            Copy(copy);
        }

        JsonValue& operator=(const JsonValue& copy)
        {
            Copy(copy);
            return *this;
        }

        JsonValue(JsonValue&& move)
        {
            memcpy(this, &move, sizeof(JsonValue));
            memset(&move, 0, sizeof(JsonValue));
        }

        ~JsonValue()
        {
            Clear();
        }

        JsonValueType GetValueType() const { return valueType; }

    private:
        void Copy(const JsonValue& copy)
        {
            memcpy(this, &copy, sizeof(JsonValue));
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
        }
        
        void Clear()
        {
            memset(this, 0, sizeof(JsonValue));
        }
        
        union
        {
            String stringValue;
            bool boolValue;
            u64 unsignedValue;
            s64 signedValue;
            f32 floatValue;
            JsonObject objectValue;
            JsonArray arrayValue;
        };

        JsonValueType valueType = JsonValueType::None;

        friend class JsonSerializer;
    };
    
} // namespace CE
