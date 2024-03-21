#pragma once

namespace CE::Widgets
{
	ENUM()
	enum class CEventType
	{
		Undefined = 0,
		CustomEvent,

		PaintEvent,

		MouseEnter,
		MouseMove,
		MouseLeave,
		MousePress,
		MouseRelease,
	};
	ENUM_CLASS(CEventType);

} // namespace CE::Widgets

#include "CEventType.rtti.h"