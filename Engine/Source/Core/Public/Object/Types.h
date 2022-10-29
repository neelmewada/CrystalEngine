#pragma once

#include "Misc/CoreDefines.h"
#include "Containers/String.h"

namespace CE
{

	enum class FieldBaseType
	{
		Uint8, Uint16, Uint32, Uint64,
		Int8, Int16, Int32, Int64,
		String,
		Enum, Struct, Class, Object
	};

	enum class FieldType
	{
		Plain,
		Pointer,
		Array,
		ArrayPtr,
	};

	class CORE_API TypeInfo
	{
	public:
		TypeInfo(FieldType type, FieldBaseType underlyingType, String underlyingTypePath)
		{}

	private:
		FieldType Type;
		FieldBaseType UnderlyingType;
		String UnderlyingTypePath;
	};

	struct PropertyInitializer
	{
		const char* Name;
		u32 Offset;
		u32 Size;
		CE::FieldType Type;
		CE::FieldBaseType BaseType;
		const char* BaseTypePath;
		const char* Attributes = nullptr;
	};

	class CORE_API PropertyType : public TypeInfo
	{
	public:
		PropertyType()
		{}

		CE_NO_COPY(PropertyType);

	protected:
		String Name;
	};

	class CORE_API StructType : public PropertyType
	{
	public:
		
	};

	class CORE_API ClassType : public StructType
	{
	public:

	};

} // namespace CE
