
#include "Core.h"

#include "../Bundle.inl"

namespace CE
{
    static HashMap<TypeId, u8> FieldTypeBytes{
        { 0, 0 },
        { TYPEID(u8), 1 },
		{ TYPEID(u16), 2 },
		{ TYPEID(u32), 3 },
		{ TYPEID(u64), 4 },
		{ TYPEID(s8), 5 },
		{ TYPEID(s16), 6 },
		{ TYPEID(s32), 7 },
		{ TYPEID(s64), 8 },
		{ TYPEID(f32), 9 },
		{ TYPEID(f64), 10 },
		{ TYPEID(b8), 11 },
        { TYPEID(String), 12 }, { TYPEID(Name), 12 }, { TYPEID(IO::Path), 12 },
        { TYPEID(Vec2), 13 },
        { TYPEID(Vec3), 14 },
        { TYPEID(Vec4), 15 },
        { TYPEID(Vec2i), 16 },
        { TYPEID(Vec3i), 17 },
        { TYPEID(Vec4i), 18 },
        { TYPEID(BinaryBlob), 0x16 },
        { TYPEID(Object), 0x17 },
        { TYPEID(ScriptDelegate<>), 0x18 },
        { TYPEID(ScriptEvent<>), 0x19 },
        { TYPEID(Uuid), 0x1B },
        { TYPEID(ObjectMap), 0x1C }
    };

    static constexpr u8 ObjectArrayFieldType = 0x13;
    static constexpr u8 StructArrayFieldType = 0x14;
    static constexpr u8 SimpleArrayFieldType = 0x15;
    static constexpr u8 StructFieldType = 0x1A;

    static HashSet<u8> Optional1Requirement = { StructFieldType, StructArrayFieldType };
    static HashSet<u8> Optional2Requirement = { SimpleArrayFieldType };

    static void SaveBundleDependencies(Stream* stream, const Array<Uuid>& bundleDependencies)
    {
        *stream << (u32)bundleDependencies.GetSize();

        for (int i = 0; i < bundleDependencies.GetSize(); ++i)
        {
            *stream << bundleDependencies[i];
        }
    }

    BundleSaveResult Bundle::SaveToDisk(const Ref<Bundle>& bundle, Ref<Object> asset, Stream* stream)
    {
        if (bundle.IsNull())
        {
            CE_LOG(Error, All, "Failed to save bundle: Bundle is NULL!");
            return BundleSaveResult::InvalidBundle;
        }
        if (bundle->IsTransient())
        {
            CE_LOG(Error, All, "Cannot not save a transient bundle: {}", bundle->GetName());
            return BundleSaveResult::InvalidBundle;
        }
        if (stream == nullptr || !stream->CanWrite())
        {
            CE_LOG(Error, All, "Failed to save bundle: Stream is either NULL or not writeable!");
            return BundleSaveResult::InvalidStream;
        }

        // 1. Fetch all object and struct types

        Array<ClassType*> schemaClasses;
        Array<StructType*> schemaStructs;

        bundle->FetchAllSchemaTypes(schemaClasses, schemaStructs);

        Array<StructType*> schemaTypes;
        HashMap<StructType*, int> schemaTypeToIndex;

        for (ClassType* schemaClass : schemaClasses)
        {
            schemaTypes.Add(schemaClass);
        }
        schemaTypes.AddRange(schemaStructs);

        for (int i = 0; i < schemaTypes.GetSize(); ++i)
        {
            schemaTypeToIndex[schemaTypes[i]] = i;
        }

        Array<Object*> objectsToSerialize{};
        if (asset == nullptr)
        {
            asset = bundle;
        }
        bundle->FetchSubObjectsRecursive(objectsToSerialize);

        HashMap<Uuid, Object*> objectReferences{};
        objectReferences.Add({ asset->GetUuid(), asset.Get() });
        asset->FetchObjectReferences(objectReferences);
        Array<Uuid> bundleDependencies{};

        for (const auto& [objectUuid, objectInstance] : objectReferences)
        {
            if (objectInstance == nullptr || objectUuid.IsNull())
                continue;
            if (objectInstance->IsTransient())
                continue;
            auto objectBundle = objectInstance->GetBundle();

            if (bundle != objectBundle && objectBundle != nullptr && !objectBundle->IsTransient())
            {
                bundleDependencies.Add(objectBundle->GetUuid());
            }
        }

        stream->SetBinaryMode(true);

        *stream << BUNDLE_MAGIC_NUMBER;

        u32 headerChecksum = 0;
        *stream << headerChecksum;

        *stream << BUNDLE_VERSION_MAJOR;
        *stream << BUNDLE_VERSION_MINOR;
        *stream << BUNDLE_VERSION_PATCH;

        u64 schemaTableOffset_Location = stream->GetCurrentPosition();
        *stream << (u64)0; // Placeholder
        u64 serializedDataOffset_Location = stream->GetCurrentPosition();
        *stream << (u64)0; // Placeholder

        *stream << bundle->GetUuid();
        *stream << bundle->GetName();

        SaveBundleDependencies(stream, bundleDependencies);

        u8 isCooked = 0;
        *stream << isCooked;

        // - Schema Table -

	    {
		    u64 curLocation = stream->GetCurrentPosition();
        	stream->Seek((s64)schemaTableOffset_Location, SeekMode::Begin);
        	*stream << curLocation;
        	stream->Seek((s64)curLocation, SeekMode::Begin);

        	SerializeSchemaTable(bundle, stream, schemaTypes, schemaTypeToIndex);
	    }

        // - Serialized Data -

        {
            u64 curLocation = stream->GetCurrentPosition();
            stream->Seek((s64)serializedDataOffset_Location, SeekMode::Begin);
            *stream << curLocation;
            stream->Seek((s64)curLocation, SeekMode::Begin);

            u64 serializedDataSize_Location = stream->GetCurrentPosition();
            *stream << (u64)0;

            *stream << (u32)objectsToSerialize.GetSize();

            for (int i = 0; i < objectsToSerialize.GetSize(); ++i)
            {
                ObjectSerializer serializer{ bundle, objectsToSerialize[i], schemaTypeToIndex };

                serializer.Serialize(stream);
            }
        }

        // - EOF -

        *stream << (u64)0; // End Of File

        return BundleSaveResult::Success;
    }

    void Bundle::OnObjectUnloaded(Object* object)
    {

    }

    bool Bundle::IsFieldSerialized(FieldType* field, StructType* schemaType)
    {
        if (field == nullptr || schemaType == nullptr || !field->IsSerialized())
            return false;

        TypeId fieldDeclId = field->GetDeclarationTypeId();

        if (field->IsObjectField() || field->IsStructField() || field->IsEnumField())
        {
            return true;
        }

        if (field->IsArrayField())
        {
            TypeId underlyingTypeId = field->GetUnderlyingTypeId();
            if (underlyingTypeId == 0)
            {
                return false;
            }

            TypeInfo* underlyingType = GetTypeInfo(underlyingTypeId);
            if (underlyingType == nullptr)
            {
                return false;
            }

            if (FieldTypeBytes.KeyExists(underlyingTypeId))
            {
                return true;
            }

            if (underlyingType != nullptr)
            {
                return underlyingType->IsObject() || underlyingType->IsStruct();
            }

            return false;
        }

        if (FieldTypeBytes.KeyExists(fieldDeclId))
        {
            return true;
        }

        return false;
    }

    void Bundle::SerializeSchemaTable(const Ref<Bundle>& bundle, Stream* stream, const Array<StructType*>& schemaTypes,
        const HashMap<StructType*, int>& schemaTypeToIndex)
    {
        u64 tableSize_Location = stream->GetCurrentPosition();
        *stream << (u64)0; // Placeholder

        // Num of schema entries
        *stream << (u32)schemaTypes.GetSize();

        for (int i = 0; i < schemaTypes.GetSize(); ++i)
        {
            StructType* schemaType = schemaTypes[i];

            u64 entrySize_Location = stream->GetCurrentPosition();
            *stream << (u32)0;

            if (schemaType->IsClass())
            {
                *stream << (u8)0; // 0 = Class
            }
            else
            {
                *stream << (u8)1; // 1 = Struct
            }

            *stream << schemaType->GetTypeName().GetString();

            *stream << (u32)0; // Class/struct version (unused)

            Array<FieldType*> serializedFields;

            for (int j = 0; j < schemaType->GetFieldCount(); ++j)
            {
                FieldType* field = schemaType->GetFieldAt(j);

                if (IsFieldSerialized(field, schemaType))
                {
                    serializedFields.Add(field);
                }
            }

            *stream << (u32)serializedFields.GetSize();

            for (int j = 0; j < serializedFields.GetSize(); ++j)
            {
                *stream << serializedFields[j]->GetName().GetString();

                SerializeFieldSchema(serializedFields[j], stream, schemaTypeToIndex);
            }

            u64 curLocation = stream->GetCurrentPosition();
            stream->Seek(entrySize_Location, SeekMode::Begin);
            *stream << (u32)(curLocation - entrySize_Location);
            stream->Seek(curLocation, SeekMode::Begin);
        }

        *stream << (u32)0; // End of schema table

        {
            u64 curLocation = stream->GetCurrentPosition();
            stream->Seek(tableSize_Location);
            *stream << (u64)(curLocation - tableSize_Location);
            stream->Seek(curLocation);
        }
    }

    void Bundle::SerializeFieldSchema(FieldType* field, Stream* stream, 
                                      const HashMap<StructType*, int>& schemaTypeToIndex)
    {
        TypeId fieldTypeId = field->GetDeclarationTypeId();
        u8 typeByte = 0;

        if (field->IsObjectField())
        {
            typeByte = FieldTypeBytes[TYPEID(Object)];
            *stream << typeByte;
        }
        else if (field->IsEnumField())
        {
            typeByte = FieldTypeBytes[TYPEID(s64)];
            *stream << typeByte;
        }
        else if (FieldTypeBytes.KeyExists(fieldTypeId))
        {
            typeByte = FieldTypeBytes[fieldTypeId];
            *stream << typeByte;
        }
        else if (field->IsStructField())
        {
            typeByte = StructFieldType;
            *stream << typeByte;

            *stream << (u32)schemaTypeToIndex.Get((StructType*)field->GetDeclarationType());
        }
        else if (field->IsArrayField())
        {
            TypeId underlyingTypeId = field->GetUnderlyingTypeId();
            TypeInfo* underlyingType = field->GetUnderlyingType();

            if (underlyingType->IsObject())
            {
                typeByte = ObjectArrayFieldType;
                *stream << typeByte;
            }
            else if (FieldTypeBytes.KeyExists(underlyingTypeId))
            {
                typeByte = SimpleArrayFieldType;
                *stream << typeByte;

                *stream << FieldTypeBytes[underlyingTypeId];
            }
            else if (underlyingType->IsStruct())
            {
                typeByte = StructArrayFieldType;
                *stream << typeByte;

                *stream << (u32)schemaTypeToIndex.Get((StructType*)underlyingType);
            }
            else
            {
                String msg = String::Format("Bundle::SaveToDisk(): Failed to serialize field: {}", field->GetName());
                CE_LOG(Error, All, msg);

                throw SerializationException(msg);
            }
        }
        else
        {
            String msg = String::Format("Bundle::SaveToDisk(): Failed to serialize field: {}", field->GetName());
            CE_LOG(Error, All, msg);

            throw SerializationException(msg);
        }
    }

    // --------------------------------------------------------
    // ObjectSerializer

    ObjectSerializer::ObjectSerializer(const Ref<Bundle>& bundle, Object* target, const HashMap<StructType*, int>& schemaTypeToIndex)
	    : bundle(bundle), target(target), schemeTypeToIndex(schemaTypeToIndex)
    {
	    
    }

    void ObjectSerializer::Serialize(Stream* stream)
    {
        u64 entrySize_Location = stream->GetCurrentPosition();
        *stream << (u64)8;

        u64 dataStartOffset_Location = stream->GetCurrentPosition();
        *stream << (u32)0;

        *stream << target->GetUuid();

        *stream << target->IsAsset();

        ClassType* classType = target->GetClass();

        int schemaIndex = schemeTypeToIndex.Get(classType);

        *stream << schemaIndex;

        *stream << target->GetPathInBundle();

        *stream << target->GetName();

        // - New header fields here -

        u64 curLocation = stream->GetCurrentPosition();
        stream->Seek(dataStartOffset_Location);
        *stream << (u32)(curLocation - dataStartOffset_Location - 4);
        stream->Seek(curLocation);

        // Data Fields

        u64 sizeOfFieldsSection_Location = stream->GetCurrentPosition();
        *stream << (u64)8; // size of ALL fields in bytes

        for (int i = 0; i < classType->GetFieldCount(); ++i)
        {
            FieldType* field = classType->GetFieldAt(i);
            if (!Bundle::IsFieldSerialized(field, classType))
                continue;

            SerializeField(field, target, stream);
        }

        curLocation = stream->GetCurrentPosition();
        stream->Seek(sizeOfFieldsSection_Location, SeekMode::Begin);
        *stream << (u64)(curLocation - sizeOfFieldsSection_Location);
        stream->Seek(curLocation);

        curLocation = stream->GetCurrentPosition();
        stream->Seek(entrySize_Location);
        *stream << (u64)(curLocation - entrySize_Location);
        stream->Seek(curLocation);
    }

#define SERIALIZE_POD_FIELD_CHAIN(PodType) \
    if (fieldTypeId == TYPEID(PodType))\
    {\
        *stream << field->GetFieldValue<PodType>(instance);\
    }

    void ObjectSerializer::SerializeField(FieldType* field, void* instance, Stream* stream)
    {
        u64 fieldSize_Location = stream->GetCurrentPosition();
        *stream << (u64)8; // Size of field in bytes (including 8 bytes for itself)

        TypeId fieldTypeId = field->GetDeclarationTypeId();
        TypeInfo* fieldType = field->GetUnderlyingType();

        if (field->IsObjectField())
        {
            Object* object = nullptr;
	        if (field->IsStrongRefCounted())
	        {
                object = field->GetFieldValue<Ref<Object>>(instance).Get();
	        }
            else if (field->IsWeakRefCounted())
            {
                object = field->GetFieldValue<WeakRef<Object>>(instance).Get();
            }
            else
            {
                object = field->GetFieldValue<Object*>(instance);
            }

            Bundle* objectBundle = nullptr;
            if (object)
            {
	            objectBundle = object->GetBundle();
            }

            if (object == nullptr || (objectBundle == nullptr))
            {
                *stream << Uuid::Zero();
            }
            else
            {
                *stream << object->GetUuid();
                *stream << objectBundle->GetUuid();
            }
        }
        else if (field->IsEnumField())
        {
            *stream << field->GetFieldEnumValue(instance);
        }
        else if (FieldTypeBytes.KeyExists(fieldTypeId))
        {
            if (fieldTypeId == TYPEID(ObjectMap))
            {
                const auto& objectStore = field->GetFieldValue<ObjectMap>(instance);

                *stream << (u32)objectStore.GetObjectCount();

                for (int i = 0; i < objectStore.GetObjectCount(); ++i)
                {
                    Ref<Object> object = objectStore.GetObjectAt(i);
                    if (object.IsNull())
                        continue;
                    Ref<Bundle> objectBundle = object->GetBundle();
                    if (objectBundle.IsNull())
                        continue;

                    *stream << object->GetUuid();
                    *stream << objectBundle->GetUuid();
                }
            }
            else if (fieldTypeId == TYPEID(String) || fieldTypeId == TYPEID(Name) || fieldTypeId == TYPEID(IO::Path))
            {
                String stringValue = field->GetFieldValueAsString(instance);
                *stream << stringValue;
            }
            else SERIALIZE_POD_FIELD_CHAIN(u8)
            else SERIALIZE_POD_FIELD_CHAIN(u16)
            else SERIALIZE_POD_FIELD_CHAIN(u32)
            else SERIALIZE_POD_FIELD_CHAIN(u64)
            else SERIALIZE_POD_FIELD_CHAIN(s8)
            else SERIALIZE_POD_FIELD_CHAIN(s16)
            else SERIALIZE_POD_FIELD_CHAIN(s32)
            else SERIALIZE_POD_FIELD_CHAIN(s64)
            else SERIALIZE_POD_FIELD_CHAIN(f32)
            else SERIALIZE_POD_FIELD_CHAIN(f64)
            else SERIALIZE_POD_FIELD_CHAIN(b8)
            else if (fieldTypeId == CE::GetTypeId<Vec2>())
            {
	            const auto& value = field->GetFieldValue<Vec2>(instance);
                *stream << value.x;
                *stream << value.y;
            }
            else if (fieldTypeId == CE::GetTypeId<Vec2i>())
            {
                const auto& value = field->GetFieldValue<Vec2i>(instance);
                *stream << value.x;
                *stream << value.y;
            }
            else if (fieldTypeId == CE::GetTypeId<Vec3>())
            {
                const auto& value = field->GetFieldValue<Vec3>(instance);
                *stream << value.x;
                *stream << value.y;
                *stream << value.z;
            }
            else if (fieldTypeId == CE::GetTypeId<Vec3i>())
            {
                const auto& value = field->GetFieldValue<Vec3i>(instance);
                *stream << value.x;
                *stream << value.y;
                *stream << value.z;
            }
            else if (fieldTypeId == CE::GetTypeId<Vec4>())
            {
                const auto& value = field->GetFieldValue<Vec4>(instance);
                *stream << value.x;
                *stream << value.y;
                *stream << value.z;
                *stream << value.w;
            }
            else if (fieldTypeId == CE::GetTypeId<Vec4i>())
            {
                const auto& value = field->GetFieldValue<Vec4i>(instance);
                *stream << value.x;
                *stream << value.y;
                *stream << value.z;
                *stream << value.w;
            }
            else if (fieldTypeId == CE::GetTypeId<Color>())
            {
                const auto& value = field->GetFieldValue<Color>(instance);
                *stream << value.r;
                *stream << value.g;
                *stream << value.b;
                *stream << value.a;
            }
            else if (fieldTypeId == TYPEID(BinaryBlob))
            {
                *stream << field->GetFieldValue<BinaryBlob>(instance);
            }
            else if (fieldTypeId == TYPEID(Uuid))
            {
                *stream << field->GetFieldValue<Uuid>(instance);
            }
            else if (fieldTypeId == TYPEID(ScriptDelegate<>))
            {
                IScriptDelegate* scriptDelegate = field->GetFieldDelegateValue(instance);
                FunctionBinding binding = scriptDelegate->GetBinding();

                Ref<Bundle> bundle = nullptr;

                if (Ref<Object> boundObject = binding.object.Lock())
                {
                    bundle = boundObject->GetBundle();

                    if (bundle.IsValid() && binding.function != nullptr)
                    {
                        *stream << boundObject->GetUuid();
                        *stream << bundle->GetUuid();
                        *stream << binding.function->GetName();
                    }
                    else
                    {
                        *stream << Uuid::Zero();
                    }
                }
                else
                {
                    *stream << Uuid::Zero();
                }
            }
            else if (fieldTypeId == TYPEID(ScriptEvent<>))
            {
                IScriptEvent* scriptEvent = field->GetFieldEventValue(instance);

                Array<FunctionBinding> bindings = scriptEvent->GetSerializableBindings();

                *stream << (u32)bindings.GetSize();

                for (const auto& binding : bindings)
                {
                    if (Ref<Object> boundObject = binding.object.Lock())
                    {
                        bundle = boundObject->GetBundle();

                        if (bundle.IsValid() && binding.function != nullptr)
                        {
                            *stream << boundObject->GetUuid();
                            *stream << bundle->GetUuid();
                            *stream << binding.function->GetName();
                        }
                        else
                        {
                            *stream << Uuid::Zero();
                        }
                    }
                    else
                    {
                        *stream << Uuid::Zero();
                    }
                }
            }
        }
        else if (field->IsStructField())
        {
            StructType* structType = (StructType*)field->GetDeclarationType();
            void* structInstance = field->GetFieldInstance(instance);

            for (int i = 0; i < structType->GetFieldCount(); ++i)
            {
                FieldType* structField = structType->GetFieldAt(i);

                if (!Bundle::IsFieldSerialized(structField, structType))
                {
                    continue;
                }

                SerializeField(structField, structInstance, stream);
            }
        }
        else if (field->IsArrayField())
        {
            TypeId underlyingTypeId = field->GetUnderlyingTypeId();
            TypeInfo* underlyingType = field->GetUnderlyingType();

            Array<FieldType> elementList = field->GetArrayFieldList(instance);

            *stream << (u32)elementList.GetSize();

            if (elementList.NotEmpty())
            {
                const Array<u8>& rawArray = field->GetFieldValue<Array<u8>>(instance);
                auto arrayInstance = (void*)&rawArray[0];

                if (underlyingType->IsObject())
                {
	                for (int i = 0; i < elementList.GetSize(); ++i)
	                {
                        SerializeField(&elementList[i], arrayInstance, stream);
	                }
                }
                else if (FieldTypeBytes.KeyExists(underlyingTypeId))
                {
                    for (int i = 0; i < elementList.GetSize(); ++i)
                    {
                        SerializeField(&elementList[i], arrayInstance, stream);
                    }
                }
                else if (underlyingType->IsStruct())
                {
                    for (int i = 0; i < elementList.GetSize(); ++i)
                    {
                        SerializeField(&elementList[i], arrayInstance, stream);
                    }
                }
                else
                {
                    String msg = String::Format("Bundle::SaveToDisk(): Failed to serialize field: {}", field->GetName());
                    CE_LOG(Error, All, msg);

                    throw SerializationException(msg);
                }
            }
        }

        u64 curLocation = stream->GetCurrentPosition();
        stream->Seek(fieldSize_Location);
        *stream << (u64)(curLocation - fieldSize_Location);
        stream->Seek(curLocation);

    }

} // namespace CE
