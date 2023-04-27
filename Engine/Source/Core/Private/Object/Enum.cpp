
#include "CoreMinimal.h"


namespace CE
{

	EnumConstant::EnumConstant(CE::Name name, CE::TypeId typeId, s64 value, u32 size, const char* attributes)
		: TypeInfo(name, attributes)
		, typeId(typeId)
		, value(value)
		, size(size)
	{

	}

	String EnumConstant::GetDisplayName()
	{
		String displayAttrib = GetLocalAttributeValue("Display");

		if (!displayAttrib.IsEmpty())
			return displayAttrib;

		return TypeInfo::GetDisplayName();
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

	EnumType::~EnumType()
	{
		TypeInfo::DeregisterType(this);
	}

	String EnumType::GetDisplayName()
	{
		String displayAttrib = GetLocalAttributeValue("Display");

		if (!displayAttrib.IsEmpty())
			return displayAttrib;

		return TypeInfo::GetDisplayName();
	}

	EnumConstant* EnumType::FindConstantWithValue(s64 value)
	{
		for (int i = 0; i < GetConstantsCount(); i++)
		{
			auto constant = GetConstant(i);
			if (constant == nullptr)
				continue;
			if (constant->GetValue() == value)
				return constant;
		}
		return nullptr;
	}


} // namespace CE

