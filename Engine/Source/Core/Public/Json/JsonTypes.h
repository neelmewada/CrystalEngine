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

		void Cleanup();

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

		void Clear();

    private:
		void Copy(const JsonValue& copy);
        
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


	// New Json API

	class JValue;

	typedef HashMap<String, JValue> JObject;
	typedef Array<JValue> JArray;

	class CORE_API JValue final
	{
	public:

		// Null value constructor
		JValue();

		JValue(const String& string);
		JValue(const char* string);
		JValue(bool boolValue);
		JValue(f64 numberValue);
		JValue(int numberValue);
		JValue(JObject jsonObject);
		JValue(JArray jsonArray);

		JValue(const JValue& copy)
		{
			Copy(copy);
		}

		JValue& operator=(const JValue& copy)
		{
			Copy(copy);
			return *this;
		}

		JValue(JValue&& move)
		{
			memcpy(this, &move, sizeof(JValue));
			memset(&move, 0, sizeof(JValue));
		}

		~JValue();

		FORCE_INLINE JsonValueType GetValueType() const { return valueType; }

		FORCE_INLINE JArray& GetArrayValue() { return arrayValue; }
		FORCE_INLINE const JArray& GetArrayValue() const { return arrayValue; }

		FORCE_INLINE JObject& GetObjectValue() { return objectValue; }
		FORCE_INLINE const JObject& GetObjectValue() const { return objectValue; }

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

		FORCE_INLINE void Insert(const JValue& arrayItem)
		{
			if (IsArrayValue())
				arrayValue.Add(arrayItem);
		}

		FORCE_INLINE bool IsContainerType() const
		{
			return IsArrayValue() || IsObjectValue();
		}

		FORCE_INLINE bool IsTerminalType() const
		{
			return !IsContainerType();
		}

		FORCE_INLINE JValue& operator[](u32 index)
		{
			if (index >= arrayValue.GetSize())
				arrayValue.Resize(index + 1);
			return arrayValue[index];
		}

		FORCE_INLINE JValue& operator[](const String& key)
		{
			if (!objectValue.KeyExists(key))
				objectValue.Add({ key, JValue() });
			return objectValue[key];
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

		void Clear();

	private:

		void Copy(const JValue& copy);

		union
		{
			String stringValue;
			bool boolValue;
			f64 numberValue;
			JObject objectValue;
			JArray arrayValue;
		};

		JsonValueType valueType = JsonValueType::None;

		friend class JsonSerializer;
	};
    
} // namespace CE
