#include "CoreMinimal.h"

namespace CE
{

	Variant::~Variant()
	{
		
	}

	void Variant::CopyFrom(const Variant& copy)
	{
		memcpy(this, &copy, sizeof(Variant));
	}

	void Variant::Free()
	{
		if (IsOfType<String>())
		{
			StringValue.Free();
		}
		else if (IsOfType<Name>())
		{
			String& string = const_cast<String&>(nameValue.GetString());
			string.Free();
		}
		else if (IsOfType<Array<u8>>())
		{
			if (IsArrayElementOfType<String>())
				stringArrayValue.Clear();
			else if (IsArrayElementOfType<Name>())
				nameArrayValue.Clear();
			else if (IsArrayElementOfType<IO::Path>())
				pathArrayValue.Clear();
			else
				rawArrayValue.Clear();
		}

		Clear();
	}
    
} // namespace CE
