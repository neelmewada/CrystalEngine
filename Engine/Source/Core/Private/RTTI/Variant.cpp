#include "CoreMinimal.h"

namespace CE
{

	Variant::~Variant()
	{
		Free();
	}

	void Variant::CopyFrom(const Variant& copy)
	{
		valueTypeId = copy.valueTypeId;
		arrayElementTypeId = copy.arrayElementTypeId;
		isPointer = copy.isPointer;
		isArray = copy.isArray;

		if (copy.IsOfType<String>())
		{
			StringValue = copy.StringValue;
		}
		else if (copy.isArray && copy.arrayElementTypeId == TYPEID(String))
		{
			stringArrayValue = copy.stringArrayValue;
		}
		else if (copy.isArray && copy.arrayElementTypeId == TYPEID(Name))
		{
			nameArrayValue = copy.nameArrayValue;
		}
		else if (copy.isArray && copy.arrayElementTypeId == TYPEID(IO::Path))
		{
			pathArrayValue = copy.pathArrayValue;
		}
		else if (copy.isArray)
		{
			rawArrayValue = copy.rawArrayValue;
		}
		else if (copy.IsOfType<IO::Path>())
		{
			pathValue = copy.pathValue;
		}
		else
		{
			memcpy(this, &copy, sizeof(Variant));
		}
	}

	void Variant::Free()
	{
		if (IsOfType<String>())
		{
			StringValue.~String();
		}
		else if (isArray && arrayElementTypeId == TYPEID(String))
		{
			stringArrayValue.~Array();
		}
		else if (isArray && arrayElementTypeId == TYPEID(Name))
		{
			nameArrayValue.~Array();
		}
		else if (isArray && arrayElementTypeId == TYPEID(IO::Path))
		{
			pathArrayValue.~Array();
		}
		else if (isArray)
		{
			rawArrayValue.~Array();
		}
		else if (IsOfType<IO::Path>())
		{
			pathValue.~Path();
		}

		memset(this, 0, sizeof(Variant));
	}

	bool Variant::IsObject() const
	{
		TypeInfo* type = CE::GetTypeInfo(valueTypeId);
		if (type)
		{
			return type->IsObject();
		}
		return false;
	}

	bool Variant::CanCastObject(TypeId castTo) const
	{
		TypeInfo* valueType = CE::GetTypeInfo(valueTypeId);
		TypeInfo* castToType = CE::GetTypeInfo(castTo);

		return valueType->IsObject() && castToType->IsObject() && valueType->IsAssignableTo(castTo);
	}

} // namespace CE
