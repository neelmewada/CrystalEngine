
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

        if (asset == nullptr)
            asset = bundle;

        HashMap<Uuid, Object*> objectsToSerialize{};
        objectsToSerialize.Add({ asset->GetUuid(), asset.Get() });
        asset->FetchObjectReferences(objectsToSerialize);
        Array<Uuid> bundleDependencies{};

        for (const auto& [objectUuid, objectInstance] : objectsToSerialize)
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

        u64 curLocation = stream->GetCurrentPosition();
        stream->Seek((s64)schemaTableOffset_Location, SeekMode::Begin);
        *stream << curLocation;
        stream->Seek((s64)curLocation, SeekMode::Begin);

        SerializeSchemaTable(bundle, stream, schemaTypes, schemaTypeToIndex);

        // - Serialized Data -



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

        if (field->IsObjectField())
        {
            return true;
        }
        if (field->IsStructField())
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
                typeByte = FieldTypeBytes[TYPEID(Object)];
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
            
        }
    }

    // --------------------------------------------------------
    // BundleSerializer

    BundleSerializer::BundleSerializer()
    {
	    
    }

    BundleSerializer::~BundleSerializer()
    {

    }
} // namespace CE
