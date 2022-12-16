
#include "Object/Enum.h"


namespace CE
{

	EnumConstant::EnumConstant(CE::Name name, CE::TypeId typeId, s64 value, u32 size, const char* attributes)
		: TypeInfo(name, attributes)
		, typeId(typeId)
		, value(value)
		, size(size)
	{

	}

	EnumType::EnumType(CE::Name name, CE::TypeId typeId, CE::TypeId underlyingTypeId, std::initializer_list<EnumConstant> constants, u32 size, const char* attributes)
		: TypeInfo(name, attributes)
		, typeId(typeId)
		, underlyingTypeId(underlyingTypeId)
		, constants(constants)
		, size(size)
	{
		TypeInfo::RegisterType(this);
	}


} // namespace CE

