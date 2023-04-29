
#include "CoreMinimal.h"


namespace CE
{
	CE::HashMap<TypeId, EnumType*> EnumType::registeredEnumsById{};
	CE::HashMap<Name, EnumType*> EnumType::registeredEnumsByName{};

	EnumConstant::EnumConstant(CE::Name name, CE::TypeId typeId, s64 value, u32 size, const char* attributes)
		: TypeInfo(name, attributes)
		, typeId(typeId)
		, value(value)
		, size(size)
	{

	}

	String EnumConstant::GetDisplayName()
	{
		String displayAttrib = GetAttributeValue("Display").GetStringValue();

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

	}

	EnumType::~EnumType()
	{

	}

	String EnumType::GetDisplayName()
	{
		String displayAttrib = GetAttributeValue("Display").GetStringValue();

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

	void EnumType::RegisterEnumType(EnumType* enumType)
	{
		if (enumType == nullptr || registeredEnumsById.KeyExists(enumType->GetTypeId()))
			return;

		registeredEnumsById.Add({ enumType->GetTypeId(), enumType });
		registeredEnumsByName.Add({ enumType->GetName(), enumType });
	}

	void EnumType::DeregisterEnumType(EnumType* enumType)
	{
		if (enumType == nullptr)
			return;

		registeredEnumsById.Remove(enumType->GetTypeId());
		registeredEnumsByName.Remove(enumType->GetName());
	}

	EnumType* EnumType::FindEnumByName(Name enumName)
	{
		if (!registeredEnumsByName.KeyExists(enumName))
			return nullptr;
		return registeredEnumsByName[enumName];
	}

	EnumType* EnumType::FindEnumById(TypeId enumId)
	{
		if (!registeredEnumsById.KeyExists(enumId))
			return nullptr;
		return registeredEnumsById[enumId];
	}


} // namespace CE

