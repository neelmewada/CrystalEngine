#pragma once

namespace CE
{
    class FieldType;
    class ObjectMap;

    class CORE_API JsonFieldSerializer
    {
    public:
        JsonFieldSerializer(TypeInfo* instanceType, void* instance, Stream* outStream);
        JsonFieldSerializer(Array<FieldType*> fields, void* instance, Stream* outStream);
        
        bool HasNext();

        bool WriteNext(Stream* stream);

        void WriteObjectReference(Stream* stream, Object* objectRef);

        FieldType* GetNext();
        
        inline bool IsValid() const
        {
            return (isMap || isArray) && rawInstance != nullptr;
        }
        
    private:
        
        Array<FieldType*> fields{};
        void* rawInstance = nullptr;
        bool isFirstWrite = true;
        bool isMap = false;
        bool isArray = false;
        
        PrettyJsonWriter writer;
    };

} // namespace CE
