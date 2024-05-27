#pragma once

namespace CE::Widgets
{
    CLASS()
    class CRYSTALWIDGETS_API CButton : public CWidget
    {
        CE_CLASS(CButton, CWidget)
    public:

        CButton();
        virtual ~CButton();

        // - Button API -

        void SetAlternateStyle(bool set) { subControl = (set ? CSubControl::Alternate : CSubControl::None); }

        void SetText(const String& text) { label->SetText(text); }

        const String& GetText() const { return label->GetText(); }

        // - Signals -

        CE_SIGNAL(OnButtonClicked, MouseButton);

        CE_SIGNAL(OnButtonLeftClicked);

    protected:

        void HandleEvent(CEvent* event) override;

    private:

        FIELD()
        CLabel* label = nullptr;

    };
    
} // namespace CE::Widgets

#include "CButton.rtti.h"