#pragma once

namespace CE::Widgets
{
    class CCheckBox;

    DECLARE_SCRIPT_EVENT(CCheckBoxEvent, CCheckBox* checkBox);

    CLASS()
    class CRYSTALWIDGETS_API CCheckBox : public CWidget
    {
        CE_CLASS(CCheckBox, CWidget)
    public:

        CCheckBox();

        bool IsChecked() const { return isChecked; }

        void SetChecked(bool checked);

        // - Events -

        FIELD()
        CCheckBoxEvent onCheckChanged{};

    protected:

        void Construct() override;

        void OnPaint(CPaintEvent* paintEvent) override;

        void HandleEvent(CEvent* event) override;

        FIELD()
        b8 isChecked = false;
    };
    
} // namespace CE::Widgets

#include "CCheckBox.rtti.h"