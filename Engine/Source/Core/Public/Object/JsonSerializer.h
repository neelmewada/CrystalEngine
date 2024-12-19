#pragma once

namespace CE
{
    class FieldType;
    class ObjectMap;

    class CORE_API JsonFieldSerializer
    {
    public:
        JsonFieldSerializer(StructType* instanceType, void* instance);
        JsonFieldSerializer(Array<Ptr<FieldType>> fields, void* instance);
        
        bool HasNext();

		int Serialize(Stream* stream);

		inline bool IsValid() const
		{
			return (isMap || isArray) && rawInstance != nullptr;
		}

        bool WriteNext(Stream* stream);

		bool WriteNext(JValue& json);

	private:

		bool WriteNext(JsonValue* json);

		Ptr<FieldType> GetNext();

		PrettyJsonWriter writer;
        
		StructType* structType = nullptr;
        Array<Ptr<FieldType>> fields{};
        void* rawInstance = nullptr;

        bool isFirstWrite = true;
        bool isMap = false;
        bool isArray = false;

		int curIndex = -1;
    };


	class CORE_API JsonFieldDeserializer
	{
	public:
		JsonFieldDeserializer(StructType* instanceType, void* instance);
		JsonFieldDeserializer(Array<Ptr<FieldType>> fields, void* instance);

		~JsonFieldDeserializer();

		bool HasNext();

		int Deserialize(Stream* stream);

		inline bool IsValid() const
		{
			return (isMap || isArray) && rawInstance != nullptr;
		}

		bool ReadNext(Stream* stream);

		bool ReadNext(const JValue& json);

	private:

		bool ReadNextField(JsonValue* jsonValue);

		bool ReadField(const Ptr<FieldType>& field, JsonValue* jsonValue);

		JsonValue* rootJson = nullptr;

		StructType* structType = nullptr;
		Array<Ptr<FieldType>> fields{};
		void* rawInstance = nullptr;

		bool isFirstRead = true;
		bool isMap = false;
		bool isArray = false;
		u32 arrayIndex = 0;
	};

} // namespace CE
