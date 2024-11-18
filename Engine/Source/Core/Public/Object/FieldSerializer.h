#pragma once

#if false

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

		friend class Bundle;
    };

    class CORE_API FieldDeserializer
    {
    public:
        FieldDeserializer(FieldType* fieldChain, void* instance, Bundle* currentBundle);
        FieldDeserializer(Array<FieldType*> fieldList, void* instance, Bundle* currentBundle);
        
        void SkipHeader(bool skip)
        {
            skipHeader = skip;
        }

        bool HasNext();
        
        bool ReadNext(Stream* stream);

        Object* ReadObjectReference(Stream* stream);
        
    private:

        Object* ResolveObjectReference(Uuid objectUuid, Uuid bundleUuid, Name bundleName, Name pathInBundle);

        Bundle* currentBundle = nullptr;

        Array<FieldType*> fields{};
        void* rawInstance = nullptr;
        
        bool skipHeader = false;

		u32 bundleMajor = 0, bundleMinor = 0;

		friend class Bundle;
    };
    
}

#endif