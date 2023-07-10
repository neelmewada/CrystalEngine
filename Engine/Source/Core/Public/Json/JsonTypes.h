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
        Boolean,
        Null
    };

    class JsonValue;
    
    typedef HashMap<String, JsonValue*> JsonObject;
    typedef Array<JsonValue*> JsonArray;

    class CORE_API JsonValue final
    {
    public:
        // Null value constructor
        explicit JsonValue();
        
        explicit JsonValue(const String& string);
        explicit JsonValue(bool boolValue);
        explicit JsonValue(f64 numberValue);
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

        ~JsonValue();

		FORCE_INLINE JsonValueType GetValueType() const { return valueType; }

		FORCE_INLINE JsonArray& GetArrayValue() { return arrayValue; }
		FORCE_INLINE const JsonArray& GetArrayValue() const { return arrayValue; }

		FORCE_INLINE JsonObject& GetObjectValue() { return objectValue; }
		FORCE_INLINE const JsonObject& GetObjectValue() const { return objectValue; }

		FORCE_INLINE bool& GetBoolValue() { return boolValue; }
		FORCE_INLINE bool GetBoolValue() const { return boolValue; }

		FORCE_INLINE f64& GetNumberValue() { return numberValue; }
		FORCE_INLINE f64 GetNumberValue() const { return numberValue; }

		FORCE_INLINE String& GetStringValue() { return stringValue; }
		FORCE_INLINE const String& GetStringValue() const { return stringValue; }

        FORCE_INLINE bool IsArrayValue() const { return valueType == JsonValueType::Array; }
        FORCE_INLINE bool IsObjectValue() const { return valueType == JsonValueType::Object; }
        FORCE_INLINE bool IsStringValue() const { return valueType == JsonValueType::String; }
        FORCE_INLINE bool IsNumberValue() const { return valueType == JsonValueType::Number; }
        FORCE_INLINE bool IsBoolValue() const { return valueType == JsonValueType::Boolean; }
        FORCE_INLINE bool IsNullValue() const { return valueType == JsonValueType::Null; }

        FORCE_INLINE bool IsContainerType() const
        {
            return IsArrayValue() || IsObjectValue();
        }

        FORCE_INLINE bool IsTerminalType() const
        {
            return !IsContainerType();
        }

        FORCE_INLINE JsonValue& operator[](u32 index)
        {
            return *arrayValue[index];
        }

        FORCE_INLINE JsonValue& operator[](const String& key)
        {
            return *objectValue[key];
        }

        FORCE_INLINE const JsonValue& operator[](u32 index) const
        {
            return *arrayValue[index];
        }

        FORCE_INLINE const JsonValue& operator[](const String& key) const
        {
            return *objectValue.Get(key);
        }

        FORCE_INLINE bool KeyExists(const String& key) const
        {
            return IsObjectValue() && objectValue.KeyExists(key);
        }

        FORCE_INLINE u32 GetSize() const
        {
            if (IsArrayValue())
            {
                return arrayValue.GetSize();
            }
            if (IsObjectValue())
            {
                return objectValue.GetSize();
            }
            return 0;
        }

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
            f64 numberValue;
            JsonObject objectValue;
            JsonArray arrayValue;
        };

        JsonValueType valueType = JsonValueType::None;

        friend class JsonSerializer;
    };
    
} // namespace CE
