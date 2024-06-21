#pragma once

namespace CE
{
    ENUM(Flags)
    enum class FButtonState : u8
    {
	    Default = 0,
        Hovered = BIT(0),
        Pressed = BIT(1),
        Disabled = BIT(2)
    };
    ENUM_CLASS_FLAGS(FButtonState);

    CLASS()
    class FUSIONCORE_API FButton : public FStyledWidget
    {
        CE_CLASS(FButton, FStyledWidget)
    public:

        FButton();

        FButtonState GetButtonState() const { return buttonState; }

        bool IsHovered() const { return EnumHasFlag(buttonState, FButtonState::Hovered); }
        bool IsPressed() const { return EnumHasFlag(buttonState, FButtonState::Pressed); }

        void HandleEvent(FEvent* event) override;

        void OnPaint(FPainter* painter) override;

    protected: // - Internal Methods -

        void SetState(FButtonState newState);

    protected: // - Fields -

        FIELD(NonSerialized)
        FButtonState buttonState = FButtonState::Default;

    protected:  // - Fusion Fields -

        FIELD()
        FVoidEvent m_OnPressed;

    public: // - Fusion Properties -

        FUSION_EVENT(OnPressed);

        FUSION_TESTS;
        FUSION_WIDGET;
    };
    
}

#include "FButton.rtti.h"
