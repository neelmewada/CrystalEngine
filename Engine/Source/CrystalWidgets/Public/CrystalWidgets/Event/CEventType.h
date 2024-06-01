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

		FocusChanged,

		KeyPress,
		KeyHeld,
		KeyRelease
	};
	ENUM_CLASS(CEventType);

	DECLARE_SCRIPT_EVENT(CVoidEvent);
	DECLARE_SCRIPT_EVENT(CMouseButtonEvent, MouseButton);

} // namespace CE::Widgets

#include "CEventType.rtti.h"