
#include "CoreMinimal.h"


namespace CE
{
	CE::HashMap<TypeId, EnumType*> EnumType::registeredEnumsById{};
	CE::HashMap<Name, EnumType*> EnumType::registeredEnumsByName{};

	EnumConstant::EnumConstant(const CE::Name& name, const CE::Name& enumFullName, CE::TypeId typeId, s64 value, u32 size, const char* attributes)
		: TypeInfo(name, attributes)
		, typeId(typeId)
		, value(value)
		, size(size)
		, enumFullName(enumFullName)
	{
		constantFullName = enumFullName.GetString() + "::" + name.GetString();
	}

	const CE::Name& EnumConstant::GetTypeName()
	{
		return constantFullName;
	}

	String EnumConstant::GetDisplayName()
	{
		String displayAttrib = GetAttribute("Display").GetStringValue();

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
		ConstructInternal();
	}

	EnumType::~EnumType()
	{

	}

	void EnumType::ConstructInternal()
	{
		if (HasAttribute("Flags"))
			enumFlags |= ENUM_FlagsEnum;
	}

	String EnumType::GetDisplayName()
	{
		String displayAttrib = GetAttribute("Display").GetStringValue();

		if (!displayAttrib.IsEmpty())
			return displayAttrib;

		return TypeInfo::GetDisplayName();
	}

	EnumConstant* EnumType::FindConstantWithName(const Name& name)
	{
		for (int i = 0; i < GetConstantsCount(); i++)
		{
			auto constant = GetConstant(i);
			if (constant == nullptr)
				continue;
			if (constant->GetName() == name)
				return constant;
		}
		return nullptr;
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

	s64 EnumType::GetConstantValue(const Name& constantName)
	{
		EnumConstant* constant = FindConstantWithName(constantName);
		
		if (constant != nullptr)
			return constant->GetValue();

		return 0;
	}

	void EnumType::RegisterEnumType(EnumType* enumType)
	{
		if (enumType == nullptr || registeredEnumsById.KeyExists(enumType->GetTypeId()))
			return;

		registeredEnumsById.Add({ enumType->GetTypeId(), enumType });
		registeredEnumsByName.Add({ enumType->GetTypeName(), enumType });
	}

	void EnumType::DeregisterEnumType(EnumType* enumType)
	{
		if (enumType == nullptr)
			return;

		registeredEnumsById.Remove(enumType->GetTypeId());
		registeredEnumsByName.Remove(enumType->GetTypeName());
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

