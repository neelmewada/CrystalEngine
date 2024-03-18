#pragma once

namespace CE::Widgets
{
	ENUM()
	enum class CEventType
	{
		Undefined = 0,
		CustomEvent,

		PaintEvent,
	};
	ENUM_CLASS(CEventType);

} // namespace CE::Widgets

#include "CEventType.rtti.h"