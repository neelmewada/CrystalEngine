#pragma once

#include "Misc/CoreDefines.h"

namespace CE
{

	enum ObjectFlags
	{
		OF_NoFlags = 0,

		OF_Transient = BIT(0),
		OF_ClassDefaultInstance = BIT(1),
		OF_TemplateInstance = BIT(2)
	};
	ENUM_CLASS_FLAGS(ObjectFlags);

}
