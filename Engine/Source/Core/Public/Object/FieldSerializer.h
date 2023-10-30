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

		friend class Package;
    };

    class CORE_API FieldDeserializer
    {
    public:
        FieldDeserializer(FieldType* fieldChain, void* instance, Package* currentPackage);
        FieldDeserializer(Array<FieldType*> fieldList, void* instance, Package* currentPackage);
        
        void SkipHeader(bool skip)
        {
            skipHeader = skip;
        }

        bool HasNext();
        
        bool ReadNext(Stream* stream);

        Object* ReadObjectReference(Stream* stream);
        
    private:

        Object* ResolveObjectReference(Uuid objectUuid, Uuid packageUuid, Name packageName, Name pathInPackage);

        Package* currentPackage = nullptr;

        Array<FieldType*> fields{};
        void* rawInstance = nullptr;
        
        bool skipHeader = false;

		u32 packageMajor = 0, packageMinor = 0;

		friend class Package;
    };
    
}
