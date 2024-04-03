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
    
} // namespace CE
