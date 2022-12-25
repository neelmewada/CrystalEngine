
#include "CoreTypes.h"
#include "Object/Serialization.h"

#include <iostream>

#define TYPE_SERIALIZE(x) (fieldType->GetTypeId() == TYPEID(x))\
{\
    const auto& fieldValue = fieldType->GetFieldValue<x>(instance);\
    emitter << fieldValue;\
    return;\
}

#define TYPE_DESERIALIZE(x) (fieldType->GetTypeId() == TYPEID(x))\
{\
    fieldType->SetFieldValue<x>(instance, node.as<x>());\
}

namespace CE
{
        
    SerializedObject::SerializedObject(const CE::TypeInfo* type, void* instance, SerializedObject* parent)
        : type(type)
        , instance(instance)
        , parent(parent)
    {
        
    }

    SerializedObject::SerializedObject(Object* instance, SerializedObject* parent)
        : type(instance != nullptr ? instance->GetType() : nullptr)
        , instance(instance)
        , parent(parent)
    {
        
    }

    SerializedObject::~SerializedObject()
    {
        
    }

    void SerializedObject::Serialize(IO::MemoryStream& outStream)
    {
        if (type == nullptr || instance == nullptr)
            return;
        
        YAML::Emitter out{};
        
        Serialize(out);
        
        const char* data = out.c_str();
        u32 dataSize = (u32)strlen(data) + 1;
        
        outStream = IO::MemoryStream(dataSize);
        outStream.Write(dataSize, data);
    }

    void SerializedObject::Serialize(YAML::Emitter& emitter)
    {
        if (type == nullptr || instance == nullptr)
            return;
        
        emitter << YAML::BeginMap;
        objectStores.Clear();
        
        if (type->IsStruct())
        {
            auto structType = (StructType*)type;
            
            // Store the TypeId of the struct
            emitter << YAML::Key << "_TypeId";
            emitter << YAML::Value << structType->GetTypeId();
            
        }
        else if (type->IsClass())
        {
            auto classType = (ClassType*)type;
            
            // Store the TypeId of the class
            emitter << YAML::Key << "_TypeId";
            emitter << YAML::Value << classType->GetTypeId();
            
            auto field = classType->GetFirstField();
            
            while (field != nullptr)
            {
                auto fieldName = field->GetName().GetCString();
                emitter << YAML::Key << fieldName;
                emitter << YAML::Value;
                SerializeField(field, emitter);
                
                field = field->GetNext();
            }
        }
        
        emitter << YAML::EndMap;
    }

    void SerializedObject::SerializeField(FieldType* fieldType, YAML::Emitter& emitter)
    {
        if (fieldType == nullptr || instance == nullptr)
        {
            emitter << YAML::Null;
            return;
        }
        
        if (fieldType->IsObjectStoreType()) // Serialize an object store
        {
            auto& objectStore = fieldType->GetFieldValue<ObjectStore>(instance);
            objectStores.Add(fieldType);
            
            emitter << YAML::BeginSeq;
            
            for (auto [uuid, object] : objectStore)
            {
                if (object == nullptr)
                    continue;
                
                SerializedObject serializedObj = SerializedObject(object->GetType(), object, this);
                serializedObj.Serialize(emitter);
            }
            
            emitter << YAML::EndSeq;
        }
        else if (fieldType->GetTypeId() == TYPEID(String))
        {
            const auto& fieldValue = fieldType->GetFieldValue<String>(instance);
            emitter << fieldValue.GetCString();
        }
        else if (fieldType->GetTypeId() == TYPEID(CE::Name))
        {
            const auto& fieldValue = fieldType->GetFieldValue<CE::Name>(instance);
            emitter << fieldValue.GetCString();
        }
        else if (fieldType->GetTypeId() == TYPEID(CE::Array<u8>))
        {
            const auto& array = fieldType->GetFieldValue<Array<u8>>(instance);
            auto elementType = GetTypeInfo(array.GetElementTypeId());
            
            if (elementType == nullptr)
            {
                emitter << YAML::Null;
                return;
            }
            
            if (elementType->IsPOD() || elementType->IsStruct() || elementType->IsEnum()) // Value types: can be serialized per byte
            {
                emitter << YAML::BeginSeq;
                emitter << elementType->GetTypeId();
                
                for (int i = 0; array.GetSize(); i++)
                {
                    emitter << array[i];
                }
                
                emitter << YAML::EndSeq;
            }
            else if (elementType->IsObject()) // Reference types: Objects are stored as pointers
            {
                Array<Object*>& objectArray = fieldType->GetFieldValue<Array<Object*>>(instance);
                
                emitter << YAML::BeginSeq;
                emitter << elementType->GetTypeId();
                
                for (int i = 0; i < objectArray.GetSize(); i++)
                {
                    Object* object = objectArray[i];
                    
                    if (object == nullptr || object->GetUuid() == 0)
                    {
                        emitter << YAML::Null;
                        continue;
                    }
                    
                    // Serialize only the Object fields: name, uuid
                    SerializedObject serializedObj = SerializedObject(Object::Type(), object);
                    serializedObj.Serialize(emitter);
                }
                
                emitter << YAML::EndSeq;
            }
        }
        else if (fieldType->IsAssignableTo(TYPEID(Object))) // Serialize an object reference
        {
            Object* object = fieldType->GetFieldValue<Object*>(instance);
            
            if (object == nullptr)
            {
                emitter << YAML::Null;
                return;
            }
            
            if (object->GetType() == nullptr)
            {
                CE_LOG(Error, All, "Object {} has nullptr TypeInfo", object->GetName());
                emitter << YAML::Null;
                return;
            }
            
            // Serialize only the object reference
            SerializedObject serializedObj = SerializedObject(Object::Type(), object, this);
            serializedObj.Serialize(emitter);
        }
        else if (fieldType->GetDeclarationType() == GetStaticType<UUID>())
        {
            const UUID& uuid = fieldType->GetFieldValue<UUID>(instance);
            
            emitter << (u64)uuid;
        }
        else if (fieldType->GetTypeId() == TYPEID(Vec2))
        {
            const Vec2& vec = fieldType->GetFieldValue<Vec2>(instance);
            
            emitter << YAML::BeginSeq;
            emitter << vec.x << vec.y;
            emitter << YAML::EndSeq;
        }
        else if (fieldType->GetTypeId() == TYPEID(Vec3))
        {
            const Vec3& vec = fieldType->GetFieldValue<Vec3>(instance);
            
            emitter << YAML::BeginSeq;
            emitter << vec.x << vec.y << vec.z;
            emitter << YAML::EndSeq;
        }
        else if (fieldType->GetTypeId() == TYPEID(Vec4))
        {
            const Vec4& vec = fieldType->GetFieldValue<Vec4>(instance);
            
            emitter << YAML::BeginSeq;
            emitter << vec.x << vec.y << vec.z << vec.w;
            emitter << YAML::EndSeq;
        }
        else if (fieldType->GetTypeId() == TYPEID(Quat))
        {
            const Quat& quat = fieldType->GetFieldValue<Quat>(instance);
            
            emitter << YAML::BeginSeq;
            emitter << quat.x << quat.y << quat.z << quat.w;
            emitter << YAML::EndSeq;
        }
        else if TYPE_SERIALIZE(u8)
        else if TYPE_SERIALIZE(u16)
        else if TYPE_SERIALIZE(u32)
        else if TYPE_SERIALIZE(u64)
        else if TYPE_SERIALIZE(s8)
        else if TYPE_SERIALIZE(s16)
        else if TYPE_SERIALIZE(s32)
        else if TYPE_SERIALIZE(s64)
        else if TYPE_SERIALIZE(f32)
        else if TYPE_SERIALIZE(f64)
        else if TYPE_SERIALIZE(bool)
        else
        {
            emitter << YAML::Null;
            return;
        }
    }

    void SerializedObject::Deserialize(IO::MemoryStream& inStream)
    {
        if (type == nullptr || instance == nullptr)
            return;
        
        YAML::Node root = YAML::Load(inStream.GetRawPointer());
        
        Deserialize(root);
    }
    
    void SerializedObject::Deserialize(YAML::Node& root)
    {
        if (type == nullptr || instance == nullptr)
            return;
        
        if (!root.IsMap())
        {
            CE_LOG(Error, All, "Serialization Error: Failed to deserialize object of type {}. YAML node passed isn't a map", type->GetName());
            return;
        }
        
        objectStores.Clear();
        
        if (type->IsStruct())
        {
            
        }
        else if (type->IsClass())
        {
            auto classType = (ClassType*)type;
            
            TypeId classTypeId = root["_TypeId"].as<TypeId>();
            
            if (classType->GetTypeId() != classTypeId)
            {
                CE_LOG(Error, All, "Serialization Error: Failed to deserialize object of type {}.\n"
                       "The serialized text data is for an object of type id {} whereas the object to deserialize data to has type id {}.", type->GetName(), classTypeId, classType->GetTypeId());
                return;
            }
            
            // Deserialize object stores first
            
            auto field = classType->GetFirstField();
            
            while (field != nullptr)
            {
                const auto& fieldName = field->GetName();
                
                if (field->IsObjectStoreType())
                {
                    YAML::Node fieldNode = root[fieldName.GetCString()];
                    
                    if (fieldNode.IsNull())
                        continue;
                    
                    auto& objectStore = field->GetFieldValue<ObjectStore>(instance);
                    
                    if (!fieldNode.IsSequence())
                    {
                        CE_LOG(Error, All, "Serialization Error: Failed to deserialize object store of name {} in class of type {}.\n"
                               "The serialized text data is not a sequence.", fieldName, type->GetName());
                        continue;
                    }
                    
                    objectStores.Add(field);
                    
                    DeserializeObjectStore(fieldNode, objectStore);
                }
                
                field = field->GetNext();
            }
            
            // Deserialize other fields next
            
            field = classType->GetFirstField();
            
            while (field != nullptr)
            {
                const auto& fieldName = field->GetName().GetCString();
                
                YAML::Node fieldNode = root[fieldName];
                
                DeserializeField(field, fieldNode);
                
                field = field->GetNext();
            }
        }
    }

    void SerializedObject::DeserializeField(FieldType* fieldType, YAML::Node& node)
    {
        if (fieldType == nullptr || instance == nullptr)
            return;
        
        if (fieldType->IsObject() && objectStores.GetSize() > 0)
        {
            // Deserialize object reference
            UUID objectUuid = node["uuid"].as<u64>();
            
            Object* object = ResolveObjectReference(objectUuid);
            
            fieldType->SetFieldValue<Object*>(instance, object);
        }
        else if (fieldType->GetTypeId() == TYPEID(String))
        {
            fieldType->SetFieldValue<CE::String>(instance, node.as<std::string>());
        }
        else if (fieldType->GetTypeId() == TYPEID(CE::Name))
        {
            fieldType->SetFieldValue<CE::Name>(instance, node.as<std::string>().c_str());
        }
        else if (fieldType->GetTypeId() == TYPEID(CE::Array<u8>))
        {
            //auto& array = fieldType->GetFieldValue<Array<u8>>(instance);
            
            // NEW CODE...
            auto& array = fieldType->GetFieldValue<Array<u8>>(instance);
            auto elementType = GetTypeInfo(array.GetElementTypeId());
            
            if (array.GetElementTypeId() != node[0].as<TypeId>())
            {
                CE_LOG(Error, All, "Failed to deserialize Array field: {}.\n"
                       "ElementType of array ({}) doesn't match with serialized element type {}", fieldType->GetName(), array.GetElementTypeId(), node[0].as<TypeId>());
                return;
            }
            
            u32 arraySize = (u32)node.size() - 1; // first element is always a TypeId of element type
            
            if (elementType == nullptr)
            {
                return;
            }
            
            if (elementType->IsPOD() || elementType->IsStruct() || elementType->IsEnum()) // Value types: can be serialized per byte
            {
                array.Clear();
                
                for (int i = 1; i < arraySize + 1; i++)
                {
                    array.Add(node[i].as<u8>());
                }
            }
            else if (elementType->IsObject()) // Reference types: Objects are stored as pointers
            {
                Array<Object*>& objectArray = fieldType->GetFieldValue<Array<Object*>>(instance);
                
                objectArray.Clear();
                
                for (int i = 1; i < arraySize + 1; i++)
                {
                    Object* objectInstance = ResolveObjectReference(node[i]["uuid"].as<u64>());
                    objectArray.Add(objectInstance);
                }
            }
        }
        else if (fieldType->GetTypeId() == TYPEID(UUID))
        {
            fieldType->SetFieldValue<UUID>(instance, UUID(node.as<u64>()));
        }
        else if (fieldType->GetTypeId() == TYPEID(Vec2))
        {
            fieldType->SetFieldValue<Vec2>(instance, {node[0].as<f32>(), node[1].as<f32>()});
        }
        else if (fieldType->GetTypeId() == TYPEID(Vec3))
        {
            fieldType->SetFieldValue<Vec3>(instance, {node[0].as<f32>(), node[1].as<f32>(), node[2].as<f32>()});
        }
        else if (fieldType->GetTypeId() == TYPEID(Vec4))
        {
            fieldType->SetFieldValue<Vec4>(instance, {node[0].as<f32>(), node[1].as<f32>(), node[2].as<f32>(), node[3].as<f32>()});
        }
        else if (fieldType->GetTypeId() == TYPEID(Quat))
        {
            fieldType->SetFieldValue<Quat>(instance, {node[0].as<f32>(), node[1].as<f32>(), node[2].as<f32>(), node[3].as<f32>()});
        }
        else if TYPE_DESERIALIZE(u8)
        else if TYPE_DESERIALIZE(u16)
        else if TYPE_DESERIALIZE(u32)
        else if TYPE_DESERIALIZE(u64)
        else if TYPE_DESERIALIZE(s8)
        else if TYPE_DESERIALIZE(s16)
        else if TYPE_DESERIALIZE(s32)
        else if TYPE_DESERIALIZE(s64)
        else if TYPE_DESERIALIZE(f32)
        else if TYPE_DESERIALIZE(f64)
        else if TYPE_DESERIALIZE(bool)
    }

    void SerializedObject::DeserializeObjectStore(YAML::Node& seqNode, ObjectStore& store)
    {
        store.Clear();
        
        // Step 1: Create default instances of all objects in the store
        for (int i = 0; i < seqNode.size(); i++)
        {
            YAML::Node node = seqNode[i];
            
            if (!node.IsMap())
            {
                continue;
            }
            
            TypeId objectTypeId = node["_TypeId"].as<TypeId>();
            const TypeInfo* objectType = GetTypeInfo(objectTypeId);
            
            if (objectType == nullptr)
                continue;
            
            if (!objectType->IsObject()) // ObjectStore should always contain classes that derive from Object class
                continue;
            
            auto classType = (ClassType*)objectType;
            
            if (!classType->CanBeInstantiated())
            {
                CE_LOG(Error, All, "Serialization Error: Failed to deserialize object store in class of type {}.\n"
                       "Object store item at index {} is of type that can't be instantiated", type->GetName(), i);
                continue;
            }
            
            Object* instance = (Object*)classType->CreateDefaultInstance();
            
            FieldType* uuidField = classType->FindFieldWithName("uuid");
            if (uuidField != nullptr)
            {
                uuidField->SetFieldValue<UUID>(instance, node["uuid"].as<u64>());
            }
            
            store.AddObject(instance);
        }
        
        // Step 2: Now deserialize every single instance of objects we created in the store
        for (int i = 0; i < seqNode.size(); i++)
        {
            YAML::Node node = seqNode[i];
            
            if (!node.IsMap())
            {
                continue;
            }
            
            TypeId objectTypeId = node["_TypeId"].as<TypeId>();
            const TypeInfo* objectType = GetTypeInfo(objectTypeId);
            
            if (objectType == nullptr)
                continue;
            
            if (!objectType->IsObject()) // ObjectStore should always contain classes that derive from Object class
                continue;
            
            auto classType = (ClassType*)objectType;
            
            if (!classType->CanBeInstantiated())
                continue;
            
            Object* object = store.GetObjectWithUuid(node["uuid"].as<u64>());
            
            SerializedObject serializedObj = SerializedObject(classType, object, this);
            serializedObj.Deserialize(node);
        }
    }
    
    Object* SerializedObject::ResolveObjectReference(UUID objectUuid)
    {
        for (int storeIdx = 0; storeIdx < objectStores.GetSize(); storeIdx++)
        {
            ObjectStore& store = objectStores[storeIdx]->GetFieldValue<ObjectStore>(instance);
            
            if (store.ObjectExistsWithUuid(objectUuid))
            {
                return store.GetObjectWithUuid(objectUuid);
            }
        }
        
        if (parent != nullptr)
            return parent->ResolveObjectReference(objectUuid);
        
        return nullptr;
    }

} // namespace CE

CE_RTTI_CLASS_IMPL(CORE_API, CE, SerializedObject)



