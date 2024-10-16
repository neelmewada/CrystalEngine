#pragma once

namespace CE::Widgets
{
	ENUM()
	enum class CEventDirection
	{
		TopToBottom = 0,
		BottomToTop
	};
	ENUM_CLASS(CEventDirection);

    STRUCT()
    struct CRYSTALWIDGETS_API CEvent
    {
        CE_STRUCT(CEvent)
    public:

		inline void Consume(CWidget* consumer)
		{
			isConsumed = true;

			if (consumer && firstConsumer == nullptr)
				firstConsumer = consumer;
			if (consumer)
				lastConsumer = consumer;
		}

		inline void StopPropagation()
		{
			stopPropagation = true;
		}

		inline void ConsumeAndStopPropagation(CWidget* consumer)
		{
			isConsumed = stopPropagation = true;

			if (consumer && firstConsumer == nullptr)
				firstConsumer = consumer;
			if (consumer)
				lastConsumer = consumer;
		}

		inline bool ShouldPropagate()
		{
			return !stopPropagation;
		}

		inline CEventType GetEventType() const
		{
			return type;
		}

		bool IsMouseEvent() const
		{
			switch (type)
			{
			case CEventType::MouseEnter:
			case CEventType::MouseLeave:
			case CEventType::MouseMove:
			case CEventType::MouseWheel:
			case CEventType::MousePress:
			case CEventType::MouseRelease:
			case CEventType::DragBegin:
			case CEventType::DragMove:
			case CEventType::DragEnd:
				return true;
			default:
				return false;
			}
		}

		bool IsDragEvent() const
		{
			switch (type)
			{
			case CEventType::DragBegin:
			case CEventType::DragMove:
			case CEventType::DragEnd:
				return true;
			default:
				return false;
			}
		}

		bool IsKeyEvent() const
		{
			switch (type)
			{
			case CEventType::KeyPress:
			case CEventType::KeyHeld:
			case CEventType::KeyRelease:
				return true;
			}

			return false;
		}

		// Reset the consumed and propagation flags
		void Reset()
		{
			isConsumed = stopPropagation = false;
		}

        FIELD()
		Name name{};

		FIELD()
		CEventType type{};
		
		FIELD()
		Struct* customType = nullptr; // Type of the custom event struct

		FIELD()
		b8 isConsumed = false;

		FIELD()
		b8 stopPropagation = false;

		FIELD()
		CWidget* sender = nullptr;

		FIELD()
		CEventDirection direction = CEventDirection::BottomToTop;

		// The first widget that consumed this event. Do not modify this directly.
		CWidget* firstConsumer = nullptr;

		// The last widget that consumed this event. Do not modify this directly.
		CWidget* lastConsumer = nullptr;
    };

	class CPainter;

	STRUCT()
	struct CRYSTALWIDGETS_API CPaintEvent : CEvent
	{
		CE_STRUCT(CPaintEvent, CEvent)
	public:

		CPaintEvent()
		{
			Super::type = CEventType::PaintEvent;
			Super::direction = CEventDirection::TopToBottom;
		}

		FIELD()
		CPainter* painter = nullptr;

	};

	STRUCT()
	struct CRYSTALWIDGETS_API CMouseEvent : CEvent
	{
		CE_STRUCT(CMouseEvent, CEvent)
	public:

		CMouseEvent()
		{
			Super::direction = CEventDirection::BottomToTop;
		}

		FIELD()
		MouseButton button{};

		FIELD()
		KeyModifier keyModifiers{};

		FIELD()
		Vec2 mousePos{}; // Mouse position in screen space

		FIELD()
		Vec2 prevMousePos{}; // Previous mouse position in screen space

		FIELD()
		Vec2 wheelDelta{};

		FIELD()
		b8 isInside = true; // Valid only for MouseRelease events

		FIELD()
		b8 isDoubleClick = false;

	};

	STRUCT()
	struct CRYSTALWIDGETS_API CDragEvent : CMouseEvent
	{
		CE_STRUCT(CDragEvent, CMouseEvent)
	public:

		CDragEvent()
		{
			
		}

		FIELD()
		CWidget* draggedWidget = nullptr;

		FIELD()
		CWidget* dropTarget = nullptr;

	};

	STRUCT()
	struct CRYSTALWIDGETS_API CFocusEvent : CEvent
	{
		CE_STRUCT(CFocusEvent, CEvent)
	public:

		CFocusEvent()
		{
			direction = CEventDirection::BottomToTop;
		}

		bool GotFocus() const { return gotFocus; }
		bool LostFocus() const { return !gotFocus; }

		FIELD()
		bool gotFocus = false;

		FIELD()
		CWidget* focusedWidget = nullptr;

	};

	STRUCT()
	struct CRYSTALWIDGETS_API CKeyEvent : CEvent
	{
		CE_STRUCT(CKeyEvent, CEvent)
	public:

		CKeyEvent()
		{
			
		}

		FIELD()
		KeyCode key = KeyCode::Unknown;

		FIELD()
		KeyModifier modifier = KeyModifier::None;


	};
    
} // namespace CE::Widgets

#include "CEvent.rtti.h"