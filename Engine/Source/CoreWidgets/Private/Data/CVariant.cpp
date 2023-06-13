#include "CoreWidgets.h"

namespace CE::Widgets
{
	CVariant::CVariant()
	{
		Clear();
	}

	CVariant::~CVariant()
	{
		if (IsOfType<String>())
		{
			stringValue.~String();
		}
		else if (IsOfType<Name>())
		{
			nameValue.~Name();
		}
	}

	void CVariant::Copy(const CVariant& copy)
	{
		Clear();

		valueTypeId = copy.valueTypeId;

		if (IsOfType<String>())
		{
			stringValue = copy.stringValue;
		}
		else if (IsOfType<Name>())
		{
			nameValue = copy.nameValue;
		}
		else
		{
			memcpy(this, &copy, sizeof(CVariant));
		}
	}

	void CVariant::Clear()
	{
		if (valueTypeId == TYPEID(String))
		{
			stringValue.~String();
		}
		else if (valueTypeId == TYPEID(Name))
		{
			nameValue.~Name();
		}

		memset(this, 0, sizeof(CVariant));
	}

} // namespace CE::Widgets

CE_RTTI_POD_IMPL(CE::Widgets, CVariant)
