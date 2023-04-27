#pragma once

#include "RTTI/RTTI.h"
#include "IO/Stream/MemoryStream.h"

#define YAML_CPP_API
#include "yaml-cpp/yaml.h"


namespace CE
{
    class SerializedObject;

    class CORE_API ICustomSerializer
    {
    public:
        virtual TypeId GetTargetTypeId() = 0;

        //virtual bool CanSerializeField(FieldType* fieldType) = 0;
        virtual bool TrySerializeField(FieldType* fieldType, YAML::Emitter& emitter, void* instance, SerializedObject* caller) = 0;

        //virtual bool CanDeserializeField(FieldType* fieldType) = 0;
        virtual bool TryDeserializeField(FieldType* fieldType, YAML::Node& node, void* instance, SerializedObject* caller) = 0;
    };
    
    class CORE_API SerializedObject
    {
    public:
        typedef SerializedObject Self;

        SerializedObject(const TypeInfo* type, void* instance, SerializedObject* parent = nullptr);
        SerializedObject(Object* instance, SerializedObject* parent = nullptr);
        virtual ~SerializedObject();
        
        void Serialize(IO::Path outFilePath);
        void Serialize(IO::FileStream& outFile);
        void Serialize(IO::MemoryStream& outStream);
        void Serialize(YAML::Emitter& emitter);
        
        virtual void SerializeField(FieldType* fieldType, YAML::Emitter& emitter);
        virtual void DeserializeField(FieldType* fieldType, YAML::Node& node);

        bool Deserialize(IO::Path inFilePath);
        bool Deserialize(IO::FileStream& inFile);
        bool Deserialize(IO::MemoryStream& inStream);
        bool Deserialize(YAML::Node& root);

        static Name DeserializeObjectName(IO::Path inFilePath);
        static Name DeserializeObjectName(IO::FileStream& inFile);
        static Name DeserializeObjectName(IO::MemoryStream& inStream);
        static Name DeserializeObjectName(YAML::Node& root);

		// - Custom Serializer API -

        static void RegisterCustomSerializer(TypeId targetTypeId, ICustomSerializer* serializer);
        static void DeregisterCustomSerializer(ICustomSerializer* serializer);
        
    protected:

        bool HasCustomSerializerFor(TypeId targetTypeId)
        {
            return customSerializers.KeyExists(targetTypeId) && customSerializers[targetTypeId].GetSize() > 0;
        }
        
        void DeserializeObjectStore(YAML::Node& seqNode, ObjectStore& store);
        
        Object* ResolveObjectReference(UUID objectUuid);
        
        SerializedObject* parent = nullptr;
        const TypeInfo* type;
        void* instance;
        
        CE::Array<FieldType*> objectStores{};

        static HashMap<TypeId, Array<ICustomSerializer*>> customSerializers;
    };

} // namespace CE


#define CE_REGISTER_SERIALIZER(TargetTypeId, CustomSerializerClass)\
CE::SerializedObject::RegisterCustomSerializer(TargetTypeId, &CustomSerializerClass::Get())

#define CE_DEREGISTER_SERIALIZER(CustomSerializerClass)\
CE::SerializedObject::DeregisterCustomSerializer(&CustomSerializerClass::Get())

