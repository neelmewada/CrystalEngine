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
		if (!stream || !IsValid())
			return 0;

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
			return 0;
		}

		return 0;
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
			else if (field->IsObjectStoreType()) // Object stores are stored as arrays
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

} // namespace CE

