#pragma once

namespace CE::Widgets
{
	ENUM(Flags)
	enum class WidgetFlags
	{
		None = 0,
	};
	ENUM_CLASS_FLAGS(WidgetFlags);

	
	/*
	*	Forward Decls
	*/

	class CStyleManager;

	/*
	*	Global Variables
	*/

	COREWIDGETS_API CStyleManager* GetStyleManager();
    
} // namespace CE::Widgets

#include "Definitions.rtti.h"
