#include "CoreMinimal.h"

namespace CE
{
	static HashMap<TypeId, u8> typeIdToFieldTypeMap{
		{ 0, 0x00 }, // Null value
		{ TYPEID(u8), 0x01 }, { TYPEID(c8), 0x01 },
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

		// String types
		{ TYPEID(String), 0x0C }, { TYPEID(Name), 0x0C }, { TYPEID(IO::Path), 0x0C }, { TYPEID(SubClassType<>), 0x0C }, 
		{ TYPEID(ClassType), 0x0C }, { TYPEID(StructType), 0x0C }, { TYPEID(EnumType), 0x0C },

		{ TYPEID(BinaryBlob), 0x0D },
		{ TYPEID(Object), 0x0E },
		{ TYPEID(HashMap<>), 0x10 },
		{ TYPEID(Array<>), 0x11 }, { TYPEID(ObjectMap), 0x11 },
		{ TYPEID(Vec2), 0x81 }, { TYPEID(Vec2i), 0x81 },
		{ TYPEID(Vec3), 0x82 }, { TYPEID(Vec3i), 0x82 },
		{ TYPEID(Vec4), 0x83 }, { TYPEID(Vec4i), 0x83 }, { TYPEID(Quat), 0x83 }, { TYPEID(Color), 0x83 },
		{ TYPEID(Matrix4x4), 0x84 },
	};

	BinarySerializer::BinarySerializer(TypeInfo* targetType, void* targetInstance)
		: targetType(targetType), instance(targetInstance)
	{
		
	}

	int BinarySerializer::Serialize(Stream* stream)
	{
		if (!stream || !IsValid() || !stream->CanWrite())
			return -1;

		stream->SetBinaryMode(true);

		auto type = targetType;

		if (type->IsClass())
		{
			isMap = true;
			Class* clazz = (Class*)type;

			clazz->OnBeforeSerialize(instance);

			*stream << typeIdToFieldTypeMap[TYPEID(HashMap<>)];

			auto headerPos = stream->GetCurrentPosition();
			*stream << (u64)0; // Total byte size (excluding itself)
			*stream << (u32)0; // Num of elements

			u32 numFields = 0;

			for (auto field = clazz->GetFirstField(); field != nullptr; field = field->GetNext())
			{
				auto fieldDeclType = field->GetDeclarationType();
				if (fieldDeclType == nullptr)
					continue;

				*stream << field->GetName();
				
				if (!SerializeField(stream, field, instance))
				{
					*stream << (u8)0; // NULL field type if field fails to serialize
				}

				numFields++;
			}

			auto curPos = stream->GetCurrentPosition();
			stream->Seek(headerPos);
			*stream << (u64)(curPos - headerPos - sizeof(u64));
			*stream << (u32)numFields;
			stream->Seek(curPos);

			return numFields;
		}
		else if (type->IsStruct())
		{
			isMap = true;
			Struct* structure = (Struct*)type;

			structure->OnBeforeSerialize(instance);

			*stream << typeIdToFieldTypeMap[TYPEID(HashMap<>)];

			auto headerPos = stream->GetCurrentPosition();
			*stream << (u64)0; // Total byte size (excluding itself)
			*stream << (u32)0; // Num of elements

			u32 numFields = 0;

			for (auto field = structure->GetFirstField(); field != nullptr; field = field->GetNext())
			{
				auto fieldDeclType = field->GetDeclarationType();
				if (fieldDeclType == nullptr)
					continue;

				*stream << field->GetName();

				if (!SerializeField(stream, field, instance))
				{
					*stream << (u8)0; // NULL field type if field fails to serialize
				}

				numFields++;
			}

			auto curPos = stream->GetCurrentPosition();
			stream->Seek(headerPos);
			*stream << (u64)(curPos - headerPos - sizeof(u64));
			*stream << (u32)numFields;
			stream->Seek(curPos);

			return numFields;
		}
		else
		{
			return -1;
		}

		return -1;
	}

	bool BinarySerializer::SerializeField(Stream* stream, Field* field, void* instance)
	{
		auto fieldDeclId = field->GetDeclarationTypeId();
		auto fieldDeclType = field->GetDeclarationType();

		if (fieldDeclType == nullptr || fieldDeclId == 0)
			return false;

		if (field->IsPODField())
		{
			if (!typeIdToFieldTypeMap.KeyExists(fieldDeclId))
			{
				return false;
			}

			*stream << typeIdToFieldTypeMap[fieldDeclId];

			if (fieldDeclId == TYPEID(s8))
				*stream << field->GetFieldValue<s8>(instance);
			else if (fieldDeclId == TYPEID(u8))
				*stream << field->GetFieldValue<u8>(instance);
			else if (fieldDeclId == TYPEID(s16))
				*stream << field->GetFieldValue<s16>(instance);
			else if (fieldDeclId == TYPEID(u16))
				*stream << field->GetFieldValue<u16>(instance);
			else if (fieldDeclId == TYPEID(s32))
				*stream << field->GetFieldValue<s32>(instance);
			else if (fieldDeclId == TYPEID(u32) || fieldDeclId == TYPEID(UUID32))
				*stream << field->GetFieldValue<u32>(instance);
			else if (fieldDeclId == TYPEID(s64))
				*stream << field->GetFieldValue<s64>(instance);
			else if (fieldDeclId == TYPEID(u64) || fieldDeclId == TYPEID(UUID))
				*stream << field->GetFieldValue<u64>(instance);
			else if (fieldDeclId == TYPEID(f32))
				*stream << field->GetFieldValue<f32>(instance);
			else if (fieldDeclId == TYPEID(f64))
				*stream << field->GetFieldValue<f64>(instance);
			else if (fieldDeclId == TYPEID(b8))
				*stream << field->GetFieldValue<b8>(instance);
			else if (fieldDeclId == TYPEID(String) || fieldDeclId == TYPEID(Name) || fieldDeclId == TYPEID(IO::Path) || fieldDeclId == TYPEID(SubClassType<>))
			{
				*stream << field->GetFieldValueAsString(instance);
			}
			else if (fieldDeclId == TYPEID(Vec2))
			{
				const Vec2& value = field->GetFieldValue<Vec2>(instance);
				*stream << value.x;
				*stream << value.y;
			}
			else if (fieldDeclId == TYPEID(Vec2i))
			{
				const Vec2i& value = field->GetFieldValue<Vec2i>(instance);
				*stream << value.x;
				*stream << value.y;
			}
			else if (fieldDeclId == TYPEID(Vec3))
			{
				const Vec3& value = field->GetFieldValue<Vec3>(instance);
				*stream << value.x;
				*stream << value.y;
				*stream << value.z;
			}
			else if (fieldDeclId == TYPEID(Vec3i))
			{
				const Vec3i& value = field->GetFieldValue<Vec3i>(instance);
				*stream << value.x;
				*stream << value.y;
				*stream << value.z;
			}
			else if (fieldDeclId == TYPEID(Vec4))
			{
				const Vec4& value = field->GetFieldValue<Vec4>(instance);
				*stream << value.x;
				*stream << value.y;
				*stream << value.z;
				*stream << value.w;
			}
			else if (fieldDeclId == TYPEID(Vec4i))
			{
				const Vec4i& value = field->GetFieldValue<Vec4i>(instance);
				*stream << value.x;
				*stream << value.y;
				*stream << value.z;
				*stream << value.w;
			}
			else if (fieldDeclId == TYPEID(Matrix4x4))
			{
				const Matrix4x4& mat = field->GetFieldValue<Matrix4x4>(instance);
				for (int i = 0; i < 4; i++)
				{
					*stream << mat[i].x;
					*stream << mat[i].y;
					*stream << mat[i].z;
					*stream << mat[i].w;
				}
			}
			else if (fieldDeclId == TYPEID(Quat))
			{
				const Quat& quat = field->GetFieldValue<Quat>(instance);
				*stream << quat.x;
				*stream << quat.y;
				*stream << quat.w;
				*stream << quat.z;
			}
			else if (fieldDeclId == TYPEID(Color))
			{
				const Color& color = field->GetFieldValue<Color>(instance);
				*stream << color.r;
				*stream << color.g;
				*stream << color.b;
				*stream << color.a;
			}
			else if (fieldDeclId == TYPEID(BinaryBlob))
			{
				const BinaryBlob& blob = field->GetFieldValue<BinaryBlob>(instance);
				*stream << blob;
			}
			else if (field->IsArrayField())
			{
				const Array<u8>& array = field->GetFieldValue<Array<u8>>(instance);

				Array<FieldType> elements = field->GetArrayFieldList(instance);

				auto headerPos = stream->GetCurrentPosition();
				*stream << (u64)0; // Data byte size
				*stream << (u32)elements.GetSize(); // Num of elements

				void* arrayInstance = nullptr;
				if (array.NonEmpty())
					arrayInstance = (void*)&array[0];

				for (int i = 0; i < elements.GetSize(); i++)
				{
					SerializeField(stream, &elements[i], arrayInstance);
				}

				auto curPos = stream->GetCurrentPosition();
				stream->Seek(headerPos);
				*stream << (u64)(curPos - headerPos - sizeof(u64));
				stream->Seek(curPos);
			}
			else if (fieldDeclId == TYPEID(ObjectMap)) // Object stores are stored as arrays
			{
				const ObjectMap& array = field->GetFieldValue<ObjectMap>(instance);

				auto headerPos = stream->GetCurrentPosition();
				*stream << (u64)0; // Data byte size
				*stream << (u32)array.GetObjectCount(); // Num of elements

				for (int i = 0; i < array.GetObjectCount(); i++)
				{
					Object* object = array.GetObjectAt(i);
					if (object == nullptr)
					{
						*stream << (u8)0; // NULL field type byte
						return true;
					}
					auto package = object->GetPackage();
					*stream << typeIdToFieldTypeMap[TYPEID(Object)]; // ObjectRef field type byte
					*stream << (u64)object->GetUuid();
					*stream << ((package != nullptr) ? (u64)package->GetUuid() : 0);
				}

				auto curPos = stream->GetCurrentPosition();
				stream->Seek(headerPos);
				*stream << (u64)(curPos - headerPos - sizeof(u64));
				stream->Seek(curPos);
			}

			return true;
		}
		else if (fieldDeclType->IsObject())
		{
			Object* object = field->GetFieldValue<Object*>(instance);
			if (object == nullptr)
			{
				*stream << (u8)0; // NULL field type byte
				return true;
			}
			auto package = object->GetPackage();
			*stream << typeIdToFieldTypeMap[TYPEID(Object)]; // Field Type byte
			*stream << (u64)object->GetUuid();
			*stream << ((package != nullptr) ? (u64)package->GetUuid() : 0);
			return true;
		}
		else if (fieldDeclType->IsStruct())
		{
			auto structType = (Struct*)fieldDeclType;
			auto structInstance = field->GetFieldInstance(instance);

			BinarySerializer serializer{ structType, structInstance };
			serializer.Serialize(stream);
			return true;
		}
		else if (fieldDeclId == TYPEID(ClassType) || fieldDeclId == TYPEID(StructType) || fieldDeclId == TYPEID(EnumType))
		{
			*stream << typeIdToFieldTypeMap[TYPEID(String)];
			*stream << field->GetFieldValueAsString(instance);
			return true;
		}

		return false;
	}

	BinaryDeserializer::BinaryDeserializer(TypeInfo* targetType, void* targetInstance)
		: targetType(targetType), instance(targetInstance)
	{
		if (targetType->IsClass())
		{
			closestParent = (Object*)targetInstance;
		}
	}

	int BinaryDeserializer::Deserialize(Stream* stream)
	{
		if (!stream || !IsValid() || !stream->CanRead())
			return -1;

		stream->SetBinaryMode(true);

		auto type = targetType;

		if (type->IsClass())
		{
			Class* clazz = (Class*)type;

			u8 typeByte = 0;
			*stream >> typeByte;

			if (typeByte != typeIdToFieldTypeMap[TYPEID(HashMap<>)])
				return -1;

			u64 totalByteSize = 0;
			u32 numElements = 0;

			*stream >> totalByteSize;
			*stream >> numElements;

			if (numElements == 0)
			{
				return 0;
			}

			for (int i = 0; i < numElements; i++)
			{
				String fieldName = "";
				*stream >> fieldName;

				Field* field = clazz->FindFieldWithName(fieldName);
				if (field == nullptr)
				{
					SkipFieldValue(stream);
					continue;
				}
				
				DeserializeField(stream, field, instance);
			}

			clazz->OnAfterDeserialize(instance);
		}
		else if (type->IsStruct())
		{
			Struct* structType = (Struct*)type;

			u8 typeByte = 0;
			*stream >> typeByte;

			if (typeByte != typeIdToFieldTypeMap[TYPEID(HashMap<>)])
				return -1;

			u64 totalByteSize = 0;
			u32 numElements = 0;

			*stream >> totalByteSize;
			*stream >> numElements;

			if (numElements == 0)
			{
				return 0;
			}

			for (int i = 0; i < numElements; i++)
			{
				String fieldName = "";
				*stream >> fieldName;

				Field* field = structType->FindFieldWithName(fieldName);
				if (field == nullptr)
				{
					SkipFieldValue(stream);
					continue;
				}

				DeserializeField(stream, field, instance);
			}

			structType->OnAfterDeserialize(instance);
		}

		return -1;
	}

	bool BinaryDeserializer::DeserializeField(Stream* stream, Field* field, void* instance)
	{
		if (field == nullptr)
		{
			SkipFieldValue(stream);
			return false;
		}

		auto fieldDeclId = field->GetDeclarationTypeId();
		auto fieldDeclType = field->GetDeclarationType();

		if (fieldDeclType == nullptr || fieldDeclId == 0)
		{
			SkipFieldValue(stream);
			return false;
		}

		u8 typeByte = 0;
		*stream >> typeByte;
		
		bool isIntData = (typeByte >= 0x01 && typeByte <= 0x08) || typeByte == 0x0B;
		bool isUnsignedInt = typeByte >= 0x01 && typeByte <= 0x04;
		bool isDecimal = (typeByte == 0x09) || (typeByte == 0x0A);
		bool isString = (typeByte == typeIdToFieldTypeMap[TYPEID(String)]);
		bool isVec2 = typeByte == typeIdToFieldTypeMap[TYPEID(Vec2)];
		bool isVec3 = typeByte == typeIdToFieldTypeMap[TYPEID(Vec3)];
		bool isVec4 = typeByte == typeIdToFieldTypeMap[TYPEID(Vec4)];
		bool isBinaryBlob = typeByte == typeIdToFieldTypeMap[TYPEID(BinaryBlob)];
		bool isArray = typeByte == typeIdToFieldTypeMap[TYPEID(Array<>)];
		bool isMap = typeByte == typeIdToFieldTypeMap[TYPEID(HashMap<>)];
		bool isObjectRef = typeByte == typeIdToFieldTypeMap[TYPEID(Object)];

		if (typeByte == 0) // NULL value
		{
			if (!field->IsSerialized())
			{
				// Do NOT serialize fields marked as NonSerialized
			}
			else if (field->IsObjectField())
			{
				field->ForceSetFieldValue<Object*>(instance, nullptr);
			}
			else if (fieldDeclId == TYPEID(b8))
			{
				field->ForceSetFieldValue<b8>(instance, false);
			}
			else if (fieldDeclId == TYPEID(c8))
			{
				field->ForceSetFieldValue<c8>(instance, 0);
			}
			else if (fieldDeclId == TYPEID(u8))
			{
				field->ForceSetFieldValue<u8>(instance, 0);
			}
			else if (fieldDeclId == TYPEID(s8))
			{
				field->ForceSetFieldValue<s8>(instance, 0);
			}
			else if (fieldDeclId == TYPEID(u16))
			{
				field->ForceSetFieldValue<u16>(instance, 0);
			}
			else if (fieldDeclId == TYPEID(s16))
			{
				field->ForceSetFieldValue<s16>(instance, 0);
			}
			else if (fieldDeclId == TYPEID(u32))
			{
				field->ForceSetFieldValue<u32>(instance, 0);
			}
			else if (fieldDeclId == TYPEID(s32))
			{
				field->ForceSetFieldValue<s32>(instance, 0);
			}
			else if (fieldDeclId == TYPEID(u64))
			{
				field->ForceSetFieldValue<u64>(instance, 0);
			}
			else if (fieldDeclId == TYPEID(s64))
			{
				field->ForceSetFieldValue<s64>(instance, 0);
			}
			else if (fieldDeclId == TYPEID(UUID))
			{
				field->ForceSetFieldValue<UUID>(instance, 0);
			}
			else if (fieldDeclId == TYPEID(UUID32))
			{
				field->ForceSetFieldValue<UUID32>(instance, 0);
			}
			else if (fieldDeclId == TYPEID(f32))
			{
				field->ForceSetFieldValue<f32>(instance, 0);
			}
			else if (fieldDeclId == TYPEID(f64))
			{
				field->ForceSetFieldValue<f64>(instance, 0);
			}
			else if (fieldDeclId == TYPEID(String))
			{
				field->ForceSetFieldValue<String>(instance, "");
			}
			else if (fieldDeclId == TYPEID(Name))
			{
				field->ForceSetFieldValue<Name>(instance, "");
			}
			else if (fieldDeclId == TYPEID(IO::Path))
			{
				field->ForceSetFieldValue<IO::Path>(instance, "");
			}
			else if (fieldDeclId == TYPEID(Vec2))
			{
				field->ForceSetFieldValue<Vec2>(instance, {});
			}
			else if (fieldDeclId == TYPEID(Vec2i))
			{
				field->ForceSetFieldValue<Vec2i>(instance, {});
			}
			else if (fieldDeclId == TYPEID(Vec3))
			{
				field->ForceSetFieldValue<Vec3>(instance, {});
			}
			else if (fieldDeclId == TYPEID(Vec3i))
			{
				field->ForceSetFieldValue<Vec3i>(instance, {});
			}
			else if (fieldDeclId == TYPEID(Vec4))
			{
				field->ForceSetFieldValue<Vec4>(instance, {});
			}
			else if (fieldDeclId == TYPEID(Color))
			{
				field->ForceSetFieldValue<Color>(instance, {});
			}
			else if (fieldDeclId == TYPEID(Vec4i))
			{
				field->ForceSetFieldValue<Vec4i>(instance, {});
			}
			else
			{
				return false;
			}

			return true;
		}
		else if (isBinaryBlob)
		{
			if (fieldDeclId == TYPEID(BinaryBlob))
			{
				BinaryBlob& blob = const_cast<BinaryBlob&>(field->GetFieldValue<BinaryBlob>(instance));
				*stream >> blob;
			}
		}
		else if (isIntData) // Serialized data is an integer
		{
			u64 unsignedInt = 0;
			s64 signedInt = 0;

			c8 c8Value = 0; u8 u8Value = 0; u16 u16Value = 0; u32 u32Value = 0; u64 u64Value = 0;
			s8 s8Value = 0; s16 s16Value = 0; s32 s32Value = 0; s64 s64Value = 0;
			if (typeByte == typeIdToFieldTypeMap[TYPEID(b8)])
			{
				b8 boolValue = false;
				*stream >> boolValue;
				unsignedInt = (u64)boolValue;
				signedInt = unsignedInt;
			}
			else if (typeByte == typeIdToFieldTypeMap[TYPEID(c8)])
			{
				*stream >> c8Value;
				unsignedInt = c8Value;
				signedInt = unsignedInt;
			}
			else if (typeByte == typeIdToFieldTypeMap[TYPEID(u8)])
			{
				*stream >> u8Value;
				unsignedInt = u8Value;
			}
			else if (typeByte == typeIdToFieldTypeMap[TYPEID(u16)])
			{
				*stream >> u16Value;
				unsignedInt = u16Value;
			}
			else if (typeByte == typeIdToFieldTypeMap[TYPEID(u32)])
			{
				*stream >> u32Value;
				unsignedInt = u32Value;
			}
			else if (typeByte == typeIdToFieldTypeMap[TYPEID(u64)])
			{
				*stream >> u64Value;
				unsignedInt = u64Value;
			}
			else if (typeByte == typeIdToFieldTypeMap[TYPEID(s8)])
			{
				*stream >> s8Value;
				signedInt = s8Value;
			}
			else if (typeByte == typeIdToFieldTypeMap[TYPEID(s16)])
			{
				*stream >> s16Value;
				signedInt = s16Value;
			}
			else if (typeByte == typeIdToFieldTypeMap[TYPEID(s32)])
			{
				*stream >> s32Value;
				signedInt = s32Value;
			}
			else if (typeByte == typeIdToFieldTypeMap[TYPEID(s64)])
			{
				*stream >> s64Value;
				signedInt = s64Value;
			}

			if (fieldDeclId == TYPEID(b8))
			{
				field->ForceSetFieldValue<b8>(instance, (isUnsignedInt ? (unsignedInt > 0) : (signedInt > 0)));
			}
			else if (fieldDeclId == TYPEID(c8))
			{
				field->ForceSetFieldValue<c8>(instance, (isUnsignedInt ? unsignedInt : signedInt));
			}
			else if (fieldDeclId == TYPEID(u8))
			{
				field->ForceSetFieldValue<u8>(instance, (isUnsignedInt ? unsignedInt : signedInt));
			}
			else if (fieldDeclId == TYPEID(s8))
			{
				field->ForceSetFieldValue<s8>(instance, (isUnsignedInt ? unsignedInt : signedInt));
			}
			else if (fieldDeclId == TYPEID(u16))
			{
				field->ForceSetFieldValue<u16>(instance, (isUnsignedInt ? unsignedInt : signedInt));
			}
			else if (fieldDeclId == TYPEID(s16))
			{
				field->ForceSetFieldValue<s16>(instance, (isUnsignedInt ? unsignedInt : signedInt));
			}
			else if (fieldDeclId == TYPEID(u32))
			{
				field->ForceSetFieldValue<u32>(instance, (isUnsignedInt ? unsignedInt : signedInt));
			}
			else if (fieldDeclId == TYPEID(s32))
			{
				field->ForceSetFieldValue<s32>(instance, (isUnsignedInt ? unsignedInt : signedInt));
			}
			else if (fieldDeclId == TYPEID(u64))
			{
				field->ForceSetFieldValue<u64>(instance, (isUnsignedInt ? unsignedInt : signedInt));
			}
			else if (fieldDeclId == TYPEID(s64))
			{
				field->ForceSetFieldValue<s64>(instance, (isUnsignedInt ? unsignedInt : signedInt));
			}
			else if (fieldDeclId == TYPEID(f32))
			{
				field->ForceSetFieldValue<f32>(instance, (isUnsignedInt ? unsignedInt : signedInt));
			}
			else if (fieldDeclId == TYPEID(f64))
			{
				field->ForceSetFieldValue<f64>(instance, (isUnsignedInt ? unsignedInt : signedInt));
			}
			else if (fieldDeclId == TYPEID(UUID))
			{
				field->ForceSetFieldValue<UUID>(instance, unsignedInt);
			}
			else if (fieldDeclId == TYPEID(UUID32))
			{
				field->ForceSetFieldValue<UUID32>(instance, (u32)unsignedInt);
			}
			else if (fieldDeclId == TYPEID(String))
			{
				if (isUnsignedInt)
					field->ForceSetFieldValue<String>(instance, String::Format("{}", unsignedInt));
				else
					field->ForceSetFieldValue<String>(instance, String::Format("{}", signedInt));
			}
			else
			{
				return false;
			}

			return true;
		}
		else if (isDecimal) // Serialized data is decimal
		{
			f32 floatValue = 0;
			f64 doubleValue = 0;

			if (typeByte == typeIdToFieldTypeMap[TYPEID(f32)])
			{
				*stream >> floatValue;
				doubleValue = floatValue;
			}
			else if (typeByte == typeIdToFieldTypeMap[TYPEID(f64)])
			{
				*stream >> doubleValue;
				floatValue = doubleValue;
			}

			if (fieldDeclId == TYPEID(b8))
			{
				field->ForceSetFieldValue<b8>(instance, floatValue > 0);
			}
			else if (fieldDeclId == TYPEID(c8))
			{
				field->ForceSetFieldValue<c8>(instance, (c8)(s64)floatValue);
			}
			else if (fieldDeclId == TYPEID(u8))
			{
				field->ForceSetFieldValue<u8>(instance, (u8)(s64)floatValue);
			}
			else if (fieldDeclId == TYPEID(s8))
			{
				field->ForceSetFieldValue<s8>(instance, (s8)(s64)floatValue);
			}
			else if (fieldDeclId == TYPEID(u16))
			{
				field->ForceSetFieldValue<u16>(instance, (u16)(s64)floatValue);
			}
			else if (fieldDeclId == TYPEID(s16))
			{
				field->ForceSetFieldValue<s16>(instance, (s16)(s64)floatValue);
			}
			else if (fieldDeclId == TYPEID(u32))
			{
				field->ForceSetFieldValue<u32>(instance, (u32)floatValue);
			}
			else if (fieldDeclId == TYPEID(s32))
			{
				field->ForceSetFieldValue<s32>(instance, (s32)floatValue);
			}
			else if (fieldDeclId == TYPEID(u64))
			{
				field->ForceSetFieldValue<u64>(instance, (u64)floatValue);
			}
			else if (fieldDeclId == TYPEID(s64))
			{
				field->ForceSetFieldValue<s64>(instance, (s64)floatValue);
			}
			else if (fieldDeclId == TYPEID(f32))
			{
				field->ForceSetFieldValue<f32>(instance, floatValue);
			}
			else if (fieldDeclId == TYPEID(f64))
			{
				field->ForceSetFieldValue<f64>(instance, doubleValue);
			}
			else
			{
				return false;
			}

			return true;
		}
		else if (isString)
		{
			String string = "";
			*stream >> string;

			if (fieldDeclId == TYPEID(String))
			{
				field->ForceSetFieldValue<String>(instance, string);
			}
			else if (fieldDeclId == TYPEID(Name))
			{
				field->ForceSetFieldValue<Name>(instance, string);
			}
			else if (fieldDeclId == TYPEID(IO::Path))
			{
				field->ForceSetFieldValue<IO::Path>(instance, string);
			}
			else if (fieldDeclId == TYPEID(SubClassType<>))
			{
				ClassType* classType = ClassType::FindClass(string);
				field->ForceSetFieldValue<SubClassType<Object>>(instance, classType);
			}
			else if (fieldDeclId == TYPEID(ClassType))
			{
				ClassType* classType = ClassType::FindClass(string);
				field->ForceSetFieldValue<ClassType*>(instance, classType);
			}
			else if (fieldDeclId == TYPEID(StructType))
			{
				StructType* structType = StructType::FindStruct(string);
				field->ForceSetFieldValue<StructType*>(instance, structType);
			}
			else if (fieldDeclId == TYPEID(EnumType))
			{
				EnumType* enumType = EnumType::FindEnum(string);
				field->ForceSetFieldValue<EnumType*>(instance, enumType);
			}
			else
			{
				return false;
			}

			return true;
		}
		else if (isVec2)
		{
			Vec2 val = Vec2();
			*stream >> val.x; *stream >> val.y;

			if (fieldDeclId == TYPEID(Vec2))
			{
				field->ForceSetFieldValue<Vec2>(instance, val);
			}
			else if (fieldDeclId == TYPEID(Vec2i))
			{
				field->ForceSetFieldValue<Vec2i>(instance, Vec2i((int)val.x, (int)val.y));
			}
			else if (fieldDeclId == TYPEID(Vec3))
			{
				field->ForceSetFieldValue<Vec3>(instance, val);
			}
			else if (fieldDeclId == TYPEID(Vec3i))
			{
				field->ForceSetFieldValue<Vec3i>(instance, Vec3i((int)val.x, (int)val.y, 0));
			}
			else if (fieldDeclId == TYPEID(Vec4))
			{
				field->ForceSetFieldValue<Vec4>(instance, val);
			}
			else if (fieldDeclId == TYPEID(Vec4i))
			{
				field->ForceSetFieldValue<Vec4i>(instance, Vec4i((int)val.x, (int)val.y, 0, 0));
			}
			else
			{
				return false;
			}

			return true;
		}
		else if (isVec3)
		{
			Vec3 val = Vec3();
			*stream >> val.x; *stream >> val.y; *stream >> val.z;

			if (fieldDeclId == TYPEID(Vec2))
			{
				field->ForceSetFieldValue<Vec2>(instance, val);
			}
			else if (fieldDeclId == TYPEID(Vec2i))
			{
				field->ForceSetFieldValue<Vec2i>(instance, Vec2i((int)val.x, (int)val.y));
			}
			else if (fieldDeclId == TYPEID(Vec3))
			{
				field->ForceSetFieldValue<Vec3>(instance, val);
			}
			else if (fieldDeclId == TYPEID(Vec3i))
			{
				field->ForceSetFieldValue<Vec3i>(instance, Vec3i((int)val.x, (int)val.y, (int)val.z));
			}
			else if (fieldDeclId == TYPEID(Vec4))
			{
				field->ForceSetFieldValue<Vec4>(instance, val);
			}
			else if (fieldDeclId == TYPEID(Vec4i))
			{
				field->ForceSetFieldValue<Vec4i>(instance, Vec4i((int)val.x, (int)val.y, (int)val.z, 0));
			}
			else if (fieldDeclId == TYPEID(Quat))
			{
				field->ForceSetFieldValue<Quat>(instance, Quat(val, 0));
			}
			else if (fieldDeclId == TYPEID(Color))
			{
				field->ForceSetFieldValue<Color>(instance, val);
			}
			else
			{
				return false;
			}

			return true;
		}
		else if (isVec4)
		{
			Vec4 val = Vec4();
			*stream >> val.x; *stream >> val.y; *stream >> val.z; *stream >> val.w;

			if (fieldDeclId == TYPEID(Vec2))
			{
				field->ForceSetFieldValue<Vec2>(instance, val);
			}
			else if (fieldDeclId == TYPEID(Vec2i))
			{
				field->ForceSetFieldValue<Vec2i>(instance, Vec2i((int)val.x, (int)val.y));
			}
			else if (fieldDeclId == TYPEID(Vec3))
			{
				field->ForceSetFieldValue<Vec3>(instance, val);
			}
			else if (fieldDeclId == TYPEID(Vec3i))
			{
				field->ForceSetFieldValue<Vec3i>(instance, Vec3i((int)val.x, (int)val.y, (int)val.z));
			}
			else if (fieldDeclId == TYPEID(Vec4))
			{
				field->ForceSetFieldValue<Vec4>(instance, val);
			}
			else if (fieldDeclId == TYPEID(Vec4i))
			{
				field->ForceSetFieldValue<Vec4i>(instance, Vec4i((int)val.x, (int)val.y, (int)val.z, (int)val.w));
			}
			else if (fieldDeclId == TYPEID(Quat))
			{
				field->ForceSetFieldValue<Quat>(instance, Quat(val.x, val.y, val.z, val.w));
			}
			else if (fieldDeclId == TYPEID(Color))
			{
				field->ForceSetFieldValue<Color>(instance, val);
			}
			else
			{
				return false;
			}

			return true;
		}
		else if (isObjectRef)
		{
			u64 objectUuid = 0;
			u64 packageUuid = 0;
			*stream >> objectUuid;
			*stream >> packageUuid;
			
			if (packageUuid != 0)
			{
				Package* refPackage = Package::LoadPackageByUuid(packageUuid);
				if (refPackage != nullptr)
				{
					Object* original = field->GetFieldValue<Object*>(instance);
					Object* object = refPackage->LoadObject(objectUuid);

					if (original != nullptr && closestParent != nullptr)
					{
						
					}

					field->ForceSetFieldValue<Object*>(instance, object);
				}
			}

			return true;
		}
		else if (isMap)
		{
			if (field->IsStructField())
			{
				StructType* structType = (StructType*)field->GetDeclarationType();
				
				BinaryDeserializer deserializer{ structType, field->GetFieldInstance(instance) };
				deserializer.closestParent = closestParent;

				stream->Seek(-1, SeekMode::Current); // Go back 1 byte, the deserializer needs to read the type byte
				deserializer.Deserialize(stream);
			}
			else
			{
				return false;
			}

			return true;
		}
		else if (isArray)
		{
			if (fieldDeclId == TYPEID(ObjectMap))
			{
				ObjectMap& array = const_cast<ObjectMap&>(field->GetFieldValue<ObjectMap>(instance));

				u64 dataSize = 0;
				u32 numElements = 0;

				auto headerPos = stream->GetCurrentPosition();
				*stream >> dataSize; // Data byte size
				*stream >> numElements; // Num of elements

				for (int i = 0; i < numElements; i++)
				{
					u8 typeByte = 0;
					*stream >> typeByte;
					if (typeByte == 0)
						continue;// Null object
					if (typeByte != typeIdToFieldTypeMap[TYPEID(Object)])
						continue;

					u64 uuid = 0; u64 packageUuid = 0;
					*stream >> uuid;
					*stream >> packageUuid;

					if (packageUuid != 0)
					{
						Package* refPackage = Package::LoadPackageByUuid(packageUuid);
						if (refPackage != nullptr)
						{
							Object* object = refPackage->LoadObject(uuid);
							array.AddObject(object);
						}
					}
				}
			}
			else if (field->IsArrayField())
			{
				u64 dataSize = 0;
				u32 numElements = 0;

				auto headerPos = stream->GetCurrentPosition();
				*stream >> dataSize; // Data byte size
				*stream >> numElements; // Num of elements

				field->ResizeArray(instance, numElements);
				Array<FieldType> elements = field->GetArrayFieldList(instance);

				const Array<u8>& array = field->GetFieldValue<Array<u8>>(instance);

				void* arrayInstance = nullptr;
				if (array.NonEmpty())
					arrayInstance = (void*)&array[0];

				for (int i = 0; i < elements.GetSize(); i++)
				{
					DeserializeField(stream, &elements[i], arrayInstance);
				}
			}
			else
			{
				return false;
			}

			return true;
		}

		return false;
	}

	void BinaryDeserializer::SkipFieldValue(Stream* stream)
	{
		u8 typeByte = 0;
		*stream >> typeByte;

		if (typeByte == 0)
			return;

		u8 bytes[16];

		switch (typeByte)
		{
		case 0x01: // u8
			stream->Read(bytes, 1);
			break;
		case 0x02: // u16
			stream->Read(bytes, 2);
			break;
		case 0x03: // u32
			stream->Read(bytes, 4);
			break;
		case 0x04: // u64
			stream->Read(bytes, 8);
			break;
		case 0x05: // s8
			stream->Read(bytes, 1);
			break;
		case 0x06: // s16
			stream->Read(bytes, 2);
			break;
		case 0x07: // s32
			stream->Read(bytes, 4);
			break;
		case 0x08: // s64
			stream->Read(bytes, 8);
			break;
		case 0x09: // f32
			stream->Read(bytes, 4);
			break;
		case 0x0A: // f64
			stream->Read(bytes, 8);
			break;
		case 0x0B: // b8
			stream->Read(bytes, 1);
			break;
		case 0x0C: // String
		{
			String str = "";
			*stream >> str;
		}
			break;
		case 0x0D: // Binary blob
		{
			BinaryBlob blob{};
			*stream >> blob;
			blob.Free();
		}
			break;
		case 0x0E: // Object Ref
		{
			u64 obj = 0;
			*stream >> obj; // Object UUID
			*stream >> obj; // Package UUID
		}
			break;
		case 0x10: // Map
		case 0x11: // Array
		{
			u64 byteSize = 0;
			*stream >> byteSize;
			stream->Seek(byteSize, SeekMode::Current);
		}
			break;
		case 0x81:
			stream->Seek(sizeof(Vec2), SeekMode::Current);
			break;
		case 0x82:
			stream->Seek(sizeof(Vec3), SeekMode::Current);
			break;
		case 0x83:
			stream->Seek(sizeof(Vec4), SeekMode::Current);
			break;
		case 0x84:
			stream->Seek(sizeof(Matrix4x4), SeekMode::Current);
			break;
		}
	}

} // namespace CE

