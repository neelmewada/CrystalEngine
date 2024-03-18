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

		inline bool ShouldPropagate()
		{
			return !isHandled || !stopPropagation;
		}

		inline CEventType GetEventType() const
		{
			return type;
		}

        FIELD()
		Name name{};

		FIELD()
		CEventType type{};

		FIELD()
		TypeId customType{}; // TypeId of the custom event struct

		FIELD()
		b8 isHandled = false;

		FIELD()
		b8 stopPropagation = false;

		FIELD()
		CWidget* sender = nullptr;

		FIELD()
		CEventDirection direction = CEventDirection::BottomToTop;
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
    
} // namespace CE::Widgets

#include "CEvent.rtti.h"