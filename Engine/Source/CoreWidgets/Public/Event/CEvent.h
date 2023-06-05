#pragma once

namespace CE::Widgets
{
	class CWidget;

	ENUM()
	enum class CEventType
	{
		Undefined = 0,
		MouseEnter,
		MouseMove,
		MouseLeave,
		MousePress,
		MouseRelease,
		MouseButtonClick,
		MouseButtonDoubleClick,
		FocusChanged
	};
	ENUM_CLASS_FLAGS(CEventType);

    STRUCT()
	struct COREWIDGETS_API CEvent
	{
		CE_STRUCT(CEvent)
	public:

		CEvent() = default;
		virtual ~CEvent() = default;

		inline void MarkHandled()
		{
			isHandled = true;
		}

		inline void StopPropagation()
		{
			stopPropagation = true;
		}

		inline void HandleAndStopPropagation()
		{
			isHandled = stopPropagation = true;
		}

		inline CEventType GetEventType() const 
		{ 
			return type; 
		}

		FIELD()
		String name{};

		FIELD()
		CEventType type{};

		FIELD()
		b8 isHandled = false;

		FIELD()
		b8 stopPropagation = false;

		FIELD()
		CWidget* sender = nullptr;
	};

	STRUCT()
	struct COREWIDGETS_API CMouseEvent : public CEvent
	{
		CE_STRUCT(CMouseEvent, CE::Widgets::CEvent)
	public:

		FIELD()
		Vec2 mousePos{};

		FIELD()
		Vec2 prevMousePos{};

		FIELD()
		int mouseButton{}; // 0 = left click | 1 = right click | 2 = middle click

	};

	STRUCT()
	struct COREWIDGETS_API CFocusEvent : public CEvent
	{
		CE_STRUCT(CFocusEvent, CE::Widgets::CEvent)
	public:

		inline bool GotFocus() const { return gotFocus; }
		inline bool LostFocus() const { return !gotFocus; }

		FIELD()
		b8 gotFocus = false;
	};

} // namespace CE::Widgets


#include "CEvent.rtti.h"
