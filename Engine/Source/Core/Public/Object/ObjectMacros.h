#pragma once

#include "Misc/CoreDefines.h"

namespace CE
{

	enum class ObjectFlags
	{
		None = 0,

		Transient = BIT(0),
		ClassDefaultInstance = BIT(1),
		TemplateInstance = BIT(2)
	};
	ENUM_CLASS_FLAGS(ObjectFlags);

}
