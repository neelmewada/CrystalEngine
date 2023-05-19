#pragma once

namespace CE
{
	class FieldType;
	class ObjectMap;

	class CORE_API FieldSerializer
	{
	public:
        FieldSerializer(FieldType* fieldChain, void* instance);
        FieldSerializer(Array<FieldType*> fieldList, void* instance);

		bool HasNext();

		bool WriteNext(Stream* stream);

		void WriteObjectReference(Stream* stream, Object* objectRef);

		FieldType* GetNext();

		void SkipHeader(bool skip);

	private:
		
		Array<FieldType*> fields{};
		void* rawInstance = nullptr;

		bool skipHeader = false;
	};

    class CORE_API FieldDeserializer
    {
    public:
        FieldDeserializer(Array<FieldType*> fieldList, void* instance);
        
        void SkipHeader(bool skip)
        {
            skipHeader = skip;
        }
        
        bool ReadNext(Stream* stream);
        
    private:
        Array<FieldType*> fields{};
        void* rawInstance = nullptr;
        
        bool skipHeader = false;
    };
	
}
