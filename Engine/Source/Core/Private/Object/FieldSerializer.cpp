
#include "CoreMinimal.h"

#include "Package.inl"

namespace CE
{

    FieldSerializer::FieldSerializer(FieldType* fieldChain, void* instance)
        : rawInstance(instance)
    {
        fields.Clear();

        FieldType* field = fieldChain;

        while (field != nullptr)
        {
            fields.Add(field);
            
            field = field->GetNext();
        }
    }

    FieldSerializer::FieldSerializer(Array<FieldType*> fieldList, void* instance)
        : rawInstance(instance), fields(fieldList)
    {

    }

    bool FieldSerializer::HasNext()
    {
        return fields.NonEmpty();
    }

    bool FieldSerializer::WriteNext(Stream* stream)
    {
        if (stream == nullptr || !stream->CanWrite() || rawInstance == nullptr || !HasNext())
            return false;

        FieldType* field = fields[0];
        TypeId fieldTypeId = field->GetDeclarationTypeId();
        TypeInfo* fieldDeclarationType = field->GetDeclarationType();
        
        if (fieldDeclarationType == nullptr || fieldTypeId == 0 || !field->IsSerialized() || field->HasAttribute("DontSerialize"))
        {
            fields.RemoveAt(0);
            return false;
        }

        if (!skipHeader)
        {
			*stream << FIELD_MAGIC_NUMBER;
            *stream << field->GetName();
            *stream << field->GetDeclarationType()->GetTypeName();
        }
        
        auto dataSizePos = stream->GetCurrentPosition();
        if (!skipHeader)
            *stream << (u32)0; // Set 0 size for now. Updated later
        
        auto dataStartPos = stream->GetCurrentPosition();
        
        if (field->IsIntegerField())
        {
            if (fieldTypeId == TYPEID(u8))
                *stream << field->GetFieldValue<u8>(rawInstance);
            else if (fieldTypeId == TYPEID(u16))
                *stream << field->GetFieldValue<u16>(rawInstance);
            else if (fieldTypeId == TYPEID(u32))
                *stream << field->GetFieldValue<u32>(rawInstance);
            else if (fieldTypeId == TYPEID(u64))
                *stream << field->GetFieldValue<u64>(rawInstance);
            else if (fieldTypeId == TYPEID(s8))
                *stream << field->GetFieldValue<s8>(rawInstance);
            else if (fieldTypeId == TYPEID(s16))
                *stream << field->GetFieldValue<s16>(rawInstance);
            else if (fieldTypeId == TYPEID(s32))
                *stream << field->GetFieldValue<s32>(rawInstance);
            else if (fieldTypeId == TYPEID(s64))
                *stream << field->GetFieldValue<s64>(rawInstance);
        }
        else if (field->IsDecimalField())
        {
            if (fieldTypeId == TYPEID(f32))
                *stream << field->GetFieldValue<f32>(rawInstance);
            else if (fieldTypeId == TYPEID(f64))
                *stream << (f32)field->GetFieldValue<f64>(rawInstance);
        }
        else if (fieldTypeId == TYPEID(UUID))
        {
            *stream << field->GetFieldValue<UUID>(rawInstance);
        }
        else if (fieldTypeId == TYPEID(String))
        {
            *stream << field->GetFieldValue<String>(rawInstance);
        }
        else if (fieldTypeId == TYPEID(Name))
        {
            *stream << field->GetFieldValue<Name>(rawInstance);
        }
        else if (fieldTypeId == TYPEID(IO::Path))
        {
            *stream << field->GetFieldValue<IO::Path>(rawInstance).GetString();
        }
		else if (fieldTypeId == TYPEID(BinaryBlob))
		{
			*stream << field->GetFieldValue<BinaryBlob>(rawInstance);
		}
		else if (fieldTypeId == TYPEID(SubClassType<Object>))
		{
			ClassType* value = field->GetFieldValue<SubClassType<Object>>(rawInstance);
			if (value == nullptr)
				*stream << "";
			else
				*stream << value->GetTypeName().GetString();
		}
        else if (field->IsArrayField())
        {
            auto& array = field->GetFieldValue<Array<u8>>(rawInstance);
            
            auto underlyingType = field->GetUnderlyingType();
            u32 arraySize = field->GetArraySize(rawInstance);

            *stream << underlyingType->GetTypeName();
            *stream << arraySize;

            u64 dataSizePos = stream->GetCurrentPosition();
            *stream << (u32)0; // Total data byte size, excluding itself

            Array<FieldType> fieldList = field->GetArrayFieldList(rawInstance);
            Array<FieldType*> fieldListPtr = fieldList.Transform<FieldType*>([](FieldType& item)-> FieldType*
            {
                return &item;
            });

            if (arraySize > 0)
            {
                void* arrayInstance = &array[0];

                FieldSerializer fieldSerializer{ fieldListPtr, arrayInstance };
                fieldSerializer.SkipHeader(true);
                while (fieldSerializer.HasNext())
                {
                    fieldSerializer.WriteNext(stream);
                }
            }

            u64 curPos = stream->GetCurrentPosition();
            stream->Seek(dataSizePos);
            *stream << (u32)(curPos - dataSizePos - sizeof(u32)); // subtract the size of the "Data Byte Size" field itself
            stream->Seek(curPos);
        }
        else if (fieldTypeId == TYPEID(ObjectMap)) // ObjectMaps are stored just like arrays
        {
            const auto& map = field->GetFieldValue<ObjectMap>(rawInstance);

            u32 mapSize = map.GetObjectCount();

            *stream << TYPENAME(Object);
            *stream << mapSize;

            u64 dataSizePos = stream->GetCurrentPosition();
            *stream << (u32)0; // Total data byte size, excluding itself

            for (const auto& [uuid, object] : map)
            {
                if (object == nullptr)
                    continue;

                WriteObjectReference(stream, object);
            }

            u64 curPos = stream->GetCurrentPosition();
            stream->Seek(dataSizePos);
            *stream << (u32)(curPos - dataSizePos - sizeof(u32)); // subtract the size of the "Data Byte Size" field itself
            stream->Seek(curPos);
        }
        else if (field->IsObjectField())
        {
            Object* object = field->GetFieldValue<Object*>(rawInstance);
            
            WriteObjectReference(stream, object);
        }
        else if (fieldDeclarationType->IsStruct())
        {
            StructType* structType = (StructType*)fieldDeclarationType;
            auto structInstance = field->GetFieldInstance(rawInstance);

            auto firstField = structType->GetFirstField();

            FieldSerializer fieldSerializer{ firstField, structInstance };
            while (fieldSerializer.HasNext())
            {
                fieldSerializer.WriteNext(stream);
            }
        }
        
        u64 curPos = stream->GetCurrentPosition();
        auto dataSize = (u32)(stream->GetCurrentPosition() - dataStartPos);
        
        if (!skipHeader)
        {
            stream->Seek(dataSizePos);
            *stream << dataSize;
            stream->Seek(curPos);
        }

        fields.RemoveAt(0);
        return true;
    }

    void FieldSerializer::WriteObjectReference(Stream* stream, Object* objectRef)
    {
        if (objectRef != nullptr && !objectRef->IsTransient()) // non-temporary object
        {
            *stream << objectRef->GetUuid();
            *stream << objectRef->GetClass()->GetTypeName();

            auto package = objectRef->GetPackage();

            if (package != nullptr)
			{
				*stream << (u64)package->GetUuid();
				*stream << package->GetPackageName();
			}
            else
			{
				*stream << (u64)0; // Package UUID: 0
				*stream << "\0"; // null package name
			}

            *stream << objectRef->GetPathInPackage();
        }
        else
        {
            *stream << (u64)0; // A Uuid of 0 means NULL object
        }
    }


    FieldType* FieldSerializer::GetNext()
    {
        if (!HasNext())
            return nullptr;

        return fields[0];
    }

    void FieldSerializer::SkipHeader(bool skip)
    {
        this->skipHeader = skip;
    }

    FieldDeserializer::FieldDeserializer(FieldType* fieldChain, void* instance, Package* currentPackage)
        : rawInstance(instance), skipHeader(false), currentPackage(currentPackage)
		, packageMajor(PACKAGE_VERSION_MAJOR), packageMinor(PACKAGE_VERSION_MINOR)
    {
        fields.Clear();

        FieldType* field = fieldChain;

        while (field != nullptr)
        {
            fields.Add(field);

            field = field->GetNext();
        }
    }

    FieldDeserializer::FieldDeserializer(Array<FieldType*> fieldList, void* instance, Package* currentPackage)
        : fields(fieldList), rawInstance(instance), skipHeader(false), currentPackage(currentPackage)
		, packageMajor(PACKAGE_VERSION_MAJOR), packageMinor(PACKAGE_VERSION_MINOR)
    {
        
    }

    bool FieldDeserializer::HasNext()
    {
        return fields.NonEmpty();
    }

    bool FieldDeserializer::ReadNext(Stream* stream)
    {
        String fieldName = "";
        Name fieldTypeName = "";
        u32 fieldDataSize = 0;

        FieldType* field = nullptr;

        if (!skipHeader)
        {
			if (packageMajor == 0 || IsVersionGreaterThanOrEqualTo(packageMajor, packageMinor, FieldMagicValue_Major, FieldMagicValue_Minor))
			{
				u32 magicValue = 0;
				*stream >> magicValue;
				if (magicValue != FIELD_MAGIC_NUMBER) // End of fields or invalid field
				{
					if (magicValue != 0)
					{
						CE_LOG(Error, All, "Invalid field entry! Expected a magic field number.");
					}
					fields.Clear();
					return false;
				}
			}

            *stream >> fieldName;
            if (fieldName.IsEmpty())
            {
                fields.Clear(); // Reached end of list
                return false;
            }

            *stream >> fieldTypeName;
            *stream >> fieldDataSize;

			s32 index = fields.IndexOf([&](FieldType* field) { return field->HasAttribute("Key") && field->GetAttribute("Key").GetStringValue() == fieldName; });

			if (index < 0)
			{
				index = fields.IndexOf([&](FieldType* field) { return field->GetName() == fieldName; });
			}

            if (index < 0 || index >= fields.GetSize() || fieldDataSize == 0)
            {
                stream->Seek(fieldDataSize, SeekMode::Current); // skip this field entry
                return false;
            }

            field = fields[index];
            fields.RemoveAt(index);

            if (field == nullptr) // Field found in serialized data but not actually present in class/struct
            {
                stream->Seek(fieldDataSize, SeekMode::Current);
                return false;
            }
            if (!fieldTypeName.IsValid()) // Invalid serialized type name
            {
                stream->Seek(fieldDataSize, SeekMode::Current);
                return false;
            }
			if (field->GetDeclarationType() == nullptr) // Field's type is unknown
			{
				stream->Seek(fieldDataSize, SeekMode::Current);
				return false;
			}
            if (fieldTypeName != field->GetDeclarationType()->GetTypeName()) // Type mismatch
            {
                stream->Seek(fieldDataSize, SeekMode::Current);
                return false;
            }
        }
        else
        {
            field = fields[0];
            fields.RemoveAt(0);
        }

        TypeId fieldTypeId = field->GetTypeId();
		auto underlyingTypeId = field->GetUnderlyingTypeId();
		auto underlyingType = field->GetUnderlyingType();

        if (field->IsIntegerField())
        {
            if (fieldTypeId == TYPEID(u8))
                *stream >> field->GetFieldValue<u8>(rawInstance);
            else if (fieldTypeId == TYPEID(u16))
                *stream >> field->GetFieldValue<u16>(rawInstance);
            else if (fieldTypeId == TYPEID(u32))
                *stream >> field->GetFieldValue<u32>(rawInstance);
            else if (fieldTypeId == TYPEID(u64))
                *stream >> field->GetFieldValue<u64>(rawInstance);
            else if (fieldTypeId == TYPEID(s8))
                *stream >> field->GetFieldValue<s8>(rawInstance);
            else if (fieldTypeId == TYPEID(s16))
                *stream >> field->GetFieldValue<s16>(rawInstance);
            else if (fieldTypeId == TYPEID(s32))
                *stream >> field->GetFieldValue<s32>(rawInstance);
            else if (fieldTypeId == TYPEID(s64))
                *stream >> field->GetFieldValue<s64>(rawInstance);
        }
        else if (field->IsDecimalField())
        {
            f32 value = 0;
            *stream >> value;
            if (fieldTypeId == TYPEID(f32))
                field->ForceSetFieldValue<f32>(rawInstance, value);
            else if (fieldTypeId == TYPEID(f64))
                field->ForceSetFieldValue<f64>(rawInstance, value);
        }
        else if (fieldTypeId == TYPEID(UUID))
        {
            *stream >> field->GetFieldValue<UUID>(rawInstance);
        }
        else if (fieldTypeId == TYPEID(String))
        {
            *stream >> field->GetFieldValue<String>(rawInstance);
        }
        else if (fieldTypeId == TYPEID(Name))
        {
            *stream >> field->GetFieldValue<Name>(rawInstance);
        }
        else if (fieldTypeId == TYPEID(IO::Path))
        {
            String pathString{};
            *stream >> pathString;
            field->ForceSetFieldValue<IO::Path>(rawInstance, pathString);
        }
		else if (fieldTypeId == TYPEID(BinaryBlob))
		{
			*stream >> field->GetFieldValue<BinaryBlob>(rawInstance);
		}
		else if (fieldTypeId == TYPEID(SubClassType<Object>) && underlyingType != nullptr && underlyingType->IsClass())
		{
			SubClassType<Object>& subClassType = field->GetFieldValue<SubClassType<Object>>(rawInstance);
			ClassType* baseClassType = (ClassType*)underlyingType;
			String classTypeName = "";
			*stream >> classTypeName;
			ClassType* classType = ClassType::FindClass(classTypeName);
			if (classType != nullptr && classType->IsSubclassOf(baseClassType))
			{
				subClassType = classType;
			}
			else
			{
				subClassType = nullptr;
			}
		}
        else if (field->IsArrayField())
        {
            auto& array = field->GetFieldValue<Array<u8>>(rawInstance);
            auto underlyingType = field->GetUnderlyingType();

            Name serializedUnderlyingTypeName{};
            u32 numElements = 0;
            u32 dataSize = 0;

            *stream >> serializedUnderlyingTypeName;
            *stream >> numElements;
            *stream >> dataSize;

            if (underlyingType->GetTypeName() != serializedUnderlyingTypeName) // Underlying type mismatch
            {
                stream->Seek(dataSize, SeekMode::Current);
                return false;
            }
            
            array.Clear();

            if (numElements > 0)
            {
                field->ResizeArray(rawInstance, numElements);

                Array<FieldType> fieldList = field->GetArrayFieldList(rawInstance);
                Array<FieldType*> fieldListPtr = fieldList.Transform<FieldType*>([](FieldType& item)-> FieldType*
                    {
                        return &item;
                    });

                void* arrayInstance = &array[0];

                FieldDeserializer fieldDeserializer{ fieldListPtr, arrayInstance, currentPackage };
                fieldDeserializer.SkipHeader(true);
                while (fieldDeserializer.HasNext())
                {
                    fieldDeserializer.ReadNext(stream);
                }
            }
        }
        else if (fieldTypeId == TYPEID(ObjectMap))
        {
            ObjectMap& map = field->GetFieldValue<ObjectMap>(rawInstance);

            Name serializedUnderlyingTypeName{};
            u32 numElements = 0;
            u32 dataSize = 0;

            *stream >> serializedUnderlyingTypeName;
            *stream >> numElements;
            *stream >> dataSize;

            map.RemoveAll();

            if (numElements > 0)
            {
                for (int i = 0; i < numElements; i++)
                {
                    Object* object = ReadObjectReference(stream);
                    if (object == nullptr)
                        continue;
                    map.AddObject(object);
                }
            }
        }
        else if (field->IsObjectField())
        {
            Object* objectRef = ReadObjectReference(stream);
            field->ForceSetFieldValue(rawInstance, objectRef);

            if (field->GetName() == "outer" && field->GetOwnerType() == TYPE(Object) && objectRef != nullptr)
            {
                objectRef->AttachSubobject((Object*)rawInstance);
            }
        }
        else if (field->GetDeclarationType()->IsStruct())
        {
            auto structInstance = field->GetFieldInstance(rawInstance);
            StructType* structType = (StructType*)field->GetDeclarationType();

            auto firstField = structType->GetFirstField();

            if (firstField != nullptr)
            {
                FieldDeserializer deserializer{ firstField, structInstance, currentPackage };

                while (deserializer.HasNext())
                {
                    deserializer.ReadNext(stream);
                }
            }
        }

        return true;
    }

    Object* FieldDeserializer::ReadObjectReference(Stream* stream)
    {
        u64 uuid = 0;
        *stream >> uuid;
        if (uuid == 0) // Uuid of 0 means nullptr, skip this entry
        {
            return nullptr;
        }

        Name objectTypeName{};
        *stream >> objectTypeName;

		u64 packageUuid = 0;
		if (IsVersionGreaterThanOrEqualTo(packageMajor, packageMinor, ObjectRefPackageUuid_Major, ObjectRefPackageUuid_Minor))
		{
			*stream >> packageUuid;
		}

        Name packageName{};
        *stream >> packageName;
        Name pathInPackage{};
        *stream >> pathInPackage;

        if (objectTypeName == TYPENAME(Package))
        {
            return Package::LoadPackage(nullptr, packageName);
        }
        else if (currentPackage != nullptr && packageName == currentPackage->GetPackageName())
        {
            return currentPackage->LoadObject(uuid);
        }
        else
        {
            return ResolveObjectReference(uuid, packageUuid, packageName, pathInPackage);
        }
    }

    Object* FieldDeserializer::ResolveObjectReference(UUID objectUuid, UUID packageUuid, Name packageName, Name pathInPackage)
    {
        Package* package = Package::LoadPackage(nullptr, packageName);
        if (package == nullptr)
            return nullptr;
        
        return package->LoadObject(objectUuid);
    }

}
