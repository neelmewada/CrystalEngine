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

	private:

		bool ReadField(FieldType* field, JsonValue* jsonValue);

		JsonValue* rootJson = nullptr;

		Array<FieldType*> fields{};
		void* rawInstance = nullptr;

		bool isFirstRead = true;
		bool isMap = false;
		bool isArray = false;
		u32 arrayIndex = 0;
	};

} // namespace CE
