
#include "Object/Serialization.h"

#include <iostream>

#define TYPEID_MATCHES(x) (fieldType->GetTypeId() == TYPEID(x)) { out << fieldType->GetFieldValue<x>(instance); }

namespace CE
{

	SerializedObject::SerializedObject(const TypeInfo* type)
		: type(type)
		, instance(nullptr)
	{
		
	}

	SerializedObject::~SerializedObject()
	{

	}

	void SerializedObject::Serialize(void* instance, IO::MemoryStream* outStream)
	{
		// Initialize an invalid memory stream
		*outStream = IO::MemoryStream();

		if (type == nullptr)
			return;

		YAML::Emitter out{};

		if (type->IsStruct())
		{
			auto structType = (StructType*)type;

			auto first = structType->GetFirstField();

			out << YAML::BeginMap;

			while (first != nullptr)
			{
				out << YAML::Key << first->GetName();
				out << YAML::Value;
				SerializeProperty(instance, first, out);

				first = first->GetNext();
			}

			out << YAML::EndMap;
		}
		else if (type->IsClass())
		{
			auto classType = (ClassType*)type;
			
			auto first = classType->GetFirstField();

			out << YAML::BeginMap;

			while (first != nullptr)
			{
				out << YAML::Key << first->GetName();
				out << YAML::Value;
				SerializeProperty(instance, first, out);

				first = first->GetNext();
			}

			out << YAML::EndMap;
		}
		else
		{
			return;
		}

		const char* str = out.c_str();
		u32 length = strlen(str);
		*outStream = IO::MemoryStream(length + 1); // null terminator at the end ; Allocates memory on heap

		outStream->Write(length + 1, str);
	}

	void SerializedObject::Deserialize(void* instance, IO::GenericStream* inStream)
	{
		if (type == nullptr)
			return;
	}

	void SerializedObject::SerializeProperty(void* instance, FieldType* fieldType, YAML::Emitter& out)
	{
		if (fieldType == nullptr || instance == nullptr)
		{
			out << YAML::Null;
			return;
		}

		auto name = fieldType->GetName().GetString();
		
		if		TYPEID_MATCHES(u8)
		else if TYPEID_MATCHES(u16)
		else if TYPEID_MATCHES(u32)
		else if TYPEID_MATCHES(u64)
		else if TYPEID_MATCHES(s8)
		else if TYPEID_MATCHES(s16)
		else if TYPEID_MATCHES(s32)
		else if TYPEID_MATCHES(s64)
		else if TYPEID_MATCHES(f32)
		else if TYPEID_MATCHES(f64)
		else if TYPEID_MATCHES(bool)
		else if (fieldType->GetTypeId() == TYPEID(String))
		{
			auto string = fieldType->GetFieldValue<String>(instance);
			out << string;
		}
		else if (fieldType->GetTypeId() == TYPEID(CE::Name))
		{
			auto name = fieldType->GetFieldValue<CE::Name>(instance);
			out << name;
		}
		else if (fieldType->GetTypeId() == TYPEID(Vec2))
		{
			auto vec = fieldType->GetFieldValue<Vec2>(instance);
			out << YAML::BeginSeq << vec.X << vec.Y << YAML::EndSeq;
		}
		else if (fieldType->GetTypeId() == TYPEID(Vec3))
		{
			auto vec = fieldType->GetFieldValue<Vec3>(instance);
			out << YAML::BeginSeq << vec.X << vec.Y << vec.Z << YAML::EndSeq;
		}
		else if (fieldType->GetTypeId() == TYPEID(Vec4))
		{
			auto vec = fieldType->GetFieldValue<Vec4>(instance);
			out << YAML::BeginSeq << vec.X << vec.Y << vec.Z << vec.W << YAML::EndSeq;
		}
		else if (fieldType->GetTypeId() == TYPEID(Quat))
		{
			auto quat = fieldType->GetFieldValue<Quat>(instance);
			out << YAML::BeginSeq << quat.X << quat.Y << quat.Z << quat.W << YAML::EndSeq;
		}
		else if (fieldType->GetTypeId() == TYPEID(Matrix4x4))
		{
			auto mat = fieldType->GetFieldValue<Matrix4x4>(instance);
			out << YAML::BeginSeq 
				<< mat.Rows[0].X << mat.Rows[0].Y << mat.Rows[0].Z << mat.Rows[0].W
				<< mat.Rows[1].X << mat.Rows[1].Y << mat.Rows[1].Z << mat.Rows[1].W
				<< mat.Rows[2].X << mat.Rows[2].Y << mat.Rows[2].Z << mat.Rows[2].W
				<< mat.Rows[3].X << mat.Rows[3].Y << mat.Rows[3].Z << mat.Rows[3].W
				<< YAML::EndSeq;
		}
		else if (fieldType->GetTypeId() == TYPEID(Array<u8>))
		{
			auto array = fieldType->GetFieldValue<Array<u8>>(instance);
			out << YAML::BeginSeq
				<< array.GetElementTypeId();

			for (int i = 0; i < array.GetSize(); i++)
			{
				out << array[i];
			}

			out << YAML::EndSeq;
		}
		else if (fieldType->GetTypeId() == TYPEID(ObjectStore<Object>)) // An object store
		{
			auto store = fieldType->GetFieldValue<ObjectStore<Object>>(instance);
			out << YAML::BeginSeq;
			
			out << YAML::EndSeq;
		}
		else
		{
			out << YAML::Null;
			return;
		}
	}

} // namespace CE

CE_RTTI_CLASS_IMPL(CORE_API, CE, SerializedObject)
