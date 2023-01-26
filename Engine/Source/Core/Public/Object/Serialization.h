#pragma once

#include "RTTI.h"
#include "IO/Stream/MemoryStream.h"

#define YAML_CPP_API
#include "yaml-cpp/yaml.h"


namespace CE
{
    
    class CORE_API SerializedObject
    {
        CE_CLASS(SerializedObject)

    public:
        SerializedObject(const TypeInfo* type, void* instance, SerializedObject* parent = nullptr);
        SerializedObject(Object* instance, SerializedObject* parent = nullptr);
        virtual ~SerializedObject();
        
        void Serialize(IO::FileStream& outFile);
        void Serialize(IO::MemoryStream& outStream);
        void Serialize(YAML::Emitter& emitter);
        
        void SerializeField(FieldType* fieldType, YAML::Emitter& emitter);
        void DeserializeField(FieldType* fieldType, YAML::Node& node);
        
        void Deserialize(IO::FileStream& inFile);
        void Deserialize(IO::MemoryStream& inStream);
        void Deserialize(YAML::Node& root);
        
    protected:
        
        void DeserializeObjectStore(YAML::Node& seqNode, ObjectStore& store);
        
        Object* ResolveObjectReference(UUID objectUuid);
        
        SerializedObject* parent = nullptr;
        const TypeInfo* type;
        void* instance;
        
        CE::Array<FieldType*> objectStores{};
    };

} // namespace CE

CE_RTTI_CLASS(CORE_API, CE, SerializedObject,
    CE_SUPER(),
    CE_DONT_INSTANTIATE,
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        
    ),
    CE_FUNCTION_LIST()
)

