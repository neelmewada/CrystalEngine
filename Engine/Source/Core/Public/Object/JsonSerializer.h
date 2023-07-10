#pragma once

namespace CE
{
    class FieldType;
    class ObjectMap;

    class CORE_API JsonFieldSerializer
    {
    public:
        JsonFieldSerializer(StructType* instanceType, void* instance);
        JsonFieldSerializer(Array<FieldType*> fields, void* instance);
        
        bool HasNext();

        bool WriteNext(Stream* stream);

		bool WriteNext(JsonValue* json);

        FieldType* GetNext();
        
        inline bool IsValid() const
        {
            return (isMap || isArray) && rawInstance != nullptr;
        }
        
    private:

		PrettyJsonWriter writer;
        
        Array<FieldType*> fields{};
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
		JsonFieldDeserializer(Array<FieldType*> fields, void* instance);

		~JsonFieldDeserializer();

		bool HasNext();

		bool ReadNext(Stream* stream);

		inline bool IsValid() const
		{
			return (isMap || isArray) && rawInstance != nullptr;
		}

		bool ReadNextField(JsonValue* jsonValue);

		bool ReadField(FieldType* field, JsonValue* jsonValue);

	private:

		JsonValue* rootJson = nullptr;

		Array<FieldType*> fields{};
		void* rawInstance = nullptr;

		bool isFirstRead = true;
		bool isMap = false;
		bool isArray = false;
		u32 arrayIndex = 0;
	};

} // namespace CE
