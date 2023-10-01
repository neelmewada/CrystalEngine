
#include "CoreMinimal.h"

#include "Package.inl"

namespace CE
{
	static HashMap<TypeId, u8> typeIdToFieldTypeMap{
		{ 0, 0x00 }, // Null value
		{ TYPEID(u8), 0x01 },
		{ TYPEID(u16), 0x02 },
		{ TYPEID(u32), 0x03 }, { TYPEID(UUID32), 0x04 },
		{ TYPEID(u64), 0x04 }, { TYPEID(UUID), 0x04 },
		{ TYPEID(s8), 0x05 },
		{ TYPEID(s16), 0x06 },
		{ TYPEID(s32), 0x07 },
		{ TYPEID(s64), 0x08 },
		{ TYPEID(f32), 0x09 },
		{ TYPEID(f64), 0x0A },
		{ TYPEID(b8), 0x0B },
		{ TYPEID(String), 0x0C }, { TYPEID(Name), 0x0C }, { TYPEID(IO::Path), 0x0C },
		{ TYPEID(BinaryBlob), 0x0D },
		{ TYPEID(Object), 0x0E },
		{ TYPEID(Array<>), 0x11 },
	};

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
            *stream << field->GetName();
        }
		
        if (field->IsIntegerField())
        {
			if (typeIdToFieldTypeMap.KeyExists(fieldTypeId))
				*stream << typeIdToFieldTypeMap[fieldTypeId];

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
			if (typeIdToFieldTypeMap.KeyExists(fieldTypeId))
				*stream << typeIdToFieldTypeMap[fieldTypeId];

            if (fieldTypeId == TYPEID(f32))
                *stream << field->GetFieldValue<f32>(rawInstance);
            else if (fieldTypeId == TYPEID(f64))
                *stream << (f32)field->GetFieldValue<f64>(rawInstance);
        }
		else if (fieldTypeId == TYPEID(bool))
		{
			if (typeIdToFieldTypeMap.KeyExists(fieldTypeId))
				*stream << typeIdToFieldTypeMap[fieldTypeId];

			*stream << field->GetFieldValue<bool>(rawInstance);
		}
        else if (fieldTypeId == TYPEID(UUID))
        {
			if (typeIdToFieldTypeMap.KeyExists(fieldTypeId))
				*stream << typeIdToFieldTypeMap[fieldTypeId];

            *stream << field->GetFieldValue<UUID>(rawInstance);
        }
        else if (fieldTypeId == TYPEID(String))
        {
			if (typeIdToFieldTypeMap.KeyExists(fieldTypeId))
				*stream << typeIdToFieldTypeMap[fieldTypeId];

            *stream << field->GetFieldValue<String>(rawInstance);
        }
        else if (fieldTypeId == TYPEID(Name))
        {
			if (typeIdToFieldTypeMap.KeyExists(fieldTypeId))
				*stream << typeIdToFieldTypeMap[fieldTypeId];

            *stream << field->GetFieldValue<Name>(rawInstance);
        }
        else if (fieldTypeId == TYPEID(IO::Path))
        {
			if (typeIdToFieldTypeMap.KeyExists(fieldTypeId))
				*stream << typeIdToFieldTypeMap[fieldTypeId];

            *stream << field->GetFieldValue<IO::Path>(rawInstance).GetString();
        }
		else if (fieldTypeId == TYPEID(BinaryBlob))
		{
			if (typeIdToFieldTypeMap.KeyExists(fieldTypeId))
				*stream << typeIdToFieldTypeMap[fieldTypeId];

			*stream << field->GetFieldValue<BinaryBlob>(rawInstance);
		}
		else if (fieldTypeId == TYPEID(SubClassType<Object>))
		{
			*stream << typeIdToFieldTypeMap[TYPEID(String)];

			ClassType* value = field->GetFieldValue<SubClassType<Object>>(rawInstance);
			if (value == nullptr)
				*stream << "";
			else
				*stream << value->GetTypeName().GetString();
		}
		else if (fieldTypeId == TYPEID(ClassType))
		{
			*stream << typeIdToFieldTypeMap[TYPEID(String)];

			ClassType* value = field->GetFieldValue<ClassType*>(rawInstance);
			if (value == nullptr)
				*stream << "";
			else
				*stream << value->GetTypeName().GetString();
		}
		else if (fieldTypeId == TYPEID(StructType))
		{
			*stream << typeIdToFieldTypeMap[TYPEID(String)];

			StructType* value = field->GetFieldValue<StructType*>(rawInstance);
			if (value == nullptr)
				*stream << "";
			else
				*stream << value->GetTypeName().GetString();
		}
		else if (fieldTypeId == TYPEID(EnumType))
		{
			*stream << typeIdToFieldTypeMap[TYPEID(String)];

			EnumType* value = field->GetFieldValue<EnumType*>(rawInstance);
			if (value == nullptr)
				*stream << "";
			else
				*stream << value->GetTypeName().GetString();
		}
        else if (field->IsArrayField())
        {
			*stream << typeIdToFieldTypeMap[TYPEID(Array<>)];

            auto& array = const_cast<Array<u8>&>(field->GetFieldValue<Array<u8>>(rawInstance));
            
            auto underlyingType = field->GetUnderlyingType();
            u32 arraySize = field->GetArraySize(rawInstance);

            u64 dataSizePos = stream->GetCurrentPosition();
            *stream << (u64)0; // Total data byte size, excluding itself

			*stream << arraySize;

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
            *stream << (u64)(curPos - dataSizePos - sizeof(u64)); // subtract the size of the "Data Byte Size" field itself
            stream->Seek(curPos);
        }
        else if (fieldTypeId == TYPEID(ObjectMap)) // ObjectMaps are stored just like arrays
        {
			*stream << typeIdToFieldTypeMap[TYPEID(Array<>)];

            const auto& map = field->GetFieldValue<ObjectMap>(rawInstance);

            u32 mapSize = map.GetObjectCount();

            u64 dataSizePos = stream->GetCurrentPosition();
            *stream << (u64)0; // Total data byte size, excluding itself

			*stream << mapSize;

            for (const auto& object : map)
            {
                if (object == nullptr)
                    continue;

                WriteObjectReference(stream, object);
            }

            u64 curPos = stream->GetCurrentPosition();
            stream->Seek(dataSizePos);
            *stream << (u64)(curPos - dataSizePos - sizeof(u64)); // subtract the size of the "Data Byte Size" field itself
            stream->Seek(curPos);
        }
        else if (field->IsObjectField())
        {
			*stream << typeIdToFieldTypeMap[TYPEID(Object)];

            Object* object = field->GetFieldValue<Object*>(rawInstance);
            
            WriteObjectReference(stream, object);
        }
        else if (fieldDeclarationType->IsStruct())
        {
			*stream << (u8)0x10;

            StructType* structType = (StructType*)fieldDeclarationType;
            auto structInstance = field->GetFieldInstance(rawInstance);

            auto firstField = structType->GetFirstField();

            FieldSerializer fieldSerializer{ firstField, structInstance };
			structType->OnBeforeSerialize(structInstance);

            while (fieldSerializer.HasNext())
            {
                fieldSerializer.WriteNext(stream);
            }
        }
		else if (fieldDeclarationType->IsEnum())
		{
			*stream << typeIdToFieldTypeMap[TYPEID(s64)];

			auto enumType = (EnumType*)fieldDeclarationType;

			*stream << field->GetFieldEnumValue(rawInstance);
		}

        fields.RemoveAt(0);
        return true;
    }

    void FieldSerializer::WriteObjectReference(Stream* stream, Object* objectRef)
    {
        if (objectRef != nullptr && !objectRef->IsTransient()) // non-temporary object
        {
			u64 sizePos = stream->GetCurrentPosition();
			*stream << (u32)0; // Data size

            *stream << objectRef->GetUuid();

            auto package = objectRef->GetPackage();

            if (package != nullptr)
			{
				*stream << (u64)package->GetUuid();
				*stream << objectRef->GetClass()->GetTypeName();
			}
            else
			{
				*stream << (u64)0; // Package UUID: 0
				*stream << objectRef->GetClass()->GetTypeName();
			}

			auto curPos = stream->GetCurrentPosition();
			stream->Seek(sizePos);
			*stream << (curPos - sizePos - sizeof(u32));
			stream->Seek(curPos);
        }
        else
        {
			*stream << (u32)0; // 0 is nullptr.
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
		auto fieldDeclarationType = field->GetDeclarationType();
		auto underlyingTypeId = field->GetUnderlyingTypeId();
		auto underlyingType = field->GetUnderlyingType();

        if (field->IsIntegerField())
        {
            if (fieldTypeId == TYPEID(u8))
                field->ForceSetFieldValue<u8>(rawInstance, stream->ReadInteger<u8>());
            else if (fieldTypeId == TYPEID(u16))
				field->ForceSetFieldValue<u16>(rawInstance, stream->ReadInteger<u16>());
            else if (fieldTypeId == TYPEID(u32))
				field->ForceSetFieldValue<u32>(rawInstance, stream->ReadInteger<u32>());
            else if (fieldTypeId == TYPEID(u64))
				field->ForceSetFieldValue<u64>(rawInstance, stream->ReadInteger<u64>());
            else if (fieldTypeId == TYPEID(s8))
				field->ForceSetFieldValue<s8>(rawInstance, stream->ReadInteger<s8>());
            else if (fieldTypeId == TYPEID(s16))
				field->ForceSetFieldValue<s16>(rawInstance, stream->ReadInteger<s16>());
            else if (fieldTypeId == TYPEID(s32))
				field->ForceSetFieldValue<s32>(rawInstance, stream->ReadInteger<s32>());
            else if (fieldTypeId == TYPEID(s64))
				field->ForceSetFieldValue<s64>(rawInstance, stream->ReadInteger<s64>());
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
		else if (fieldTypeId == TYPEID(bool))
		{
			bool value = 0;
			*stream >> value;
			field->ForceSetFieldValue<bool>(rawInstance, value);
		}
        else if (fieldTypeId == TYPEID(UUID))
        {
			field->ForceSetFieldValue<UUID>(rawInstance, stream->ReadInteger<u64>());
        }
        else if (fieldTypeId == TYPEID(String))
        {
			String value = "";
			*stream >> value;
			field->ForceSetFieldValue<String>(rawInstance, value);
        }
        else if (fieldTypeId == TYPEID(Name))
        {
			Name value = "";
			*stream >> value;
			field->ForceSetFieldValue<Name>(rawInstance, value);
        }
        else if (fieldTypeId == TYPEID(IO::Path))
        {
            String pathString{};
            *stream >> pathString;
            field->ForceSetFieldValue<IO::Path>(rawInstance, pathString);
        }
		else if (fieldTypeId == TYPEID(BinaryBlob))
		{
			BinaryBlob& blob = const_cast<BinaryBlob&>(field->GetFieldValue<BinaryBlob>(rawInstance));
			*stream >> blob;
		}
		else if (fieldTypeId == TYPEID(SubClassType<Object>) && underlyingType != nullptr && underlyingType->IsClass())
		{
			SubClassType<Object>& subClassType = const_cast<SubClassType<Object>&>(field->GetFieldValue<SubClassType<Object>>(rawInstance));
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
		else if (fieldTypeId == TYPEID(ClassType))
		{
			String typeName{};
			*stream >> typeName;
			ClassType* value = ClassType::FindClass(typeName);
			field->ForceSetFieldValue(rawInstance, value);
		}
		else if (fieldTypeId == TYPEID(StructType))
		{
			String typeName{};
			*stream >> typeName;
			StructType* value = StructType::FindStruct(typeName);
			field->ForceSetFieldValue(rawInstance, value);
		}
		else if (fieldTypeId == TYPEID(EnumType))
		{
			String typeName{};
			*stream >> typeName;
			EnumType* value = EnumType::FindEnum(typeName);
			field->ForceSetFieldValue(rawInstance, value);
		}
        else if (field->IsArrayField())
        {
			auto& array = const_cast<Array<u8>&>(field->GetFieldValue<Array<u8>>(rawInstance));
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
			ObjectMap& map = const_cast<ObjectMap&>(field->GetFieldValue<ObjectMap>(rawInstance));

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

				structType->OnAfterDeserialize(structInstance);
            }
        }
		else if (fieldDeclarationType->IsEnum())
		{
			auto enumType = (EnumType*)fieldDeclarationType;

			s64 enumValue = 0;
			*stream >> enumValue;
			field->ForceSetFieldEnumValue(rawInstance, enumValue);
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
