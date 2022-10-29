#pragma once

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

} // namespace CE
