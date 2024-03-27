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

        // - Signals -

        CE_SIGNAL(OnButtonClicked, CButton*, MouseButton);

    protected:

        void HandleEvent(CEvent* event) override;

    crystalwidgets_internal:


    };
    
} // namespace CE::Widgets

#include "CButton.rtti.h"