#pragma once

namespace CE
{
    class FWidget;

    ENUM()
    enum class FEventType
    {
	    None = 0,

        MouseEnter,
        MouseMove,
        MouseLeave,
        MousePress,
        MouseRelease,
    };
    ENUM_CLASS(FEventType);

    ENUM()
    enum class FEventDirection
    {
	    BottomToTop = 0,
        TopToBottom
    };
    ENUM_CLASS(FEventDirection);

    STRUCT()
    struct FUSIONCORE_API FEvent
    {
        CE_STRUCT(FEvent)
    public:

        virtual ~FEvent() {}

        FIELD()
        FEventType type = FEventType::None;

        FIELD()
        FEventDirection direction = FEventDirection::BottomToTop;

        FIELD()
        FWidget* sender = nullptr;

        FIELD()
        FWidget* consumer = nullptr;

        bool consumed = false;

        bool stopPropagation = false;

        void Reset()
        {
            consumer = nullptr;
            consumed = stopPropagation = false;
        }

        void Consume(FWidget* comsumer)
        {
            this->consumer = consumer;
            consumed = true;
        }

        void ConsumeAndStopPropagation(FWidget* consumer)
        {
            Consume(consumer);
            stopPropagation = true;
        }

        bool IsMouseEvent() const
        {
	        switch (type)
	        {
	        case FEventType::MouseEnter:
	        case FEventType::MouseMove:
            case FEventType::MouseLeave:
            case FEventType::MousePress:
            case FEventType::MouseRelease:
                return true;
            default:
                return false;
	        }
        }
    };

    STRUCT()
    struct FUSIONCORE_API FMouseEvent : FEvent
    {
        CE_STRUCT(FMouseEvent, FEvent)
    public:

        //! @brief Mouse position in global space (i.e. context space)
        FIELD()
        Vec2 mousePosition;

        FIELD()
        Vec2 prevMousePosition;

        FIELD()
        MouseButtonMask mouseButtons = MouseButtonMask::None;

        FIELD()
        b8 isInside = false;

    };
    
} // namespace CE

#include "FEvent.rtti.h"