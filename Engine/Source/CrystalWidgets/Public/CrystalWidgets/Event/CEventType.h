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
		MouseWheel,
		MouseLeave,
		MousePress,
		MouseRelease,

		DragBegin,
		DragMove,
		DragEnd,
	};
	ENUM_CLASS(CEventType);

} // namespace CE::Widgets

#include "CEventType.rtti.h"