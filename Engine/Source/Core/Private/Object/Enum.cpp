
#include "Object/Enum.h"


namespace CE
{

	EnumConstant::EnumConstant(CE::Name name, CE::TypeId typeId, s64 value, const char* attributes)
		: TypeInfo(name, attributes)
		, TypeId(typeId)
		, Value(value)
	{

	}

	EnumType::EnumType(CE::Name name, CE::TypeId typeId, std::initializer_list<EnumConstant> constants, const char* attributes)
		: TypeInfo(name, attributes)
		, TypeId(typeId)
		, Constants(constants)
	{
		TypeInfo::RegisterType(this);
	}


} // namespace CE

