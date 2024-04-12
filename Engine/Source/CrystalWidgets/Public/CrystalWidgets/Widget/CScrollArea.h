#pragma once

namespace CE::Widgets
{
    CLASS()
    class CRYSTALWIDGETS_API CScrollArea : public CWidget
    {
        CE_CLASS(CScrollArea, CWidget)
    public:

        CScrollArea();
        virtual ~CScrollArea();

        void SetAllowVerticalScroll(bool allow);

        void SetAllowHorizontalScroll(bool allow);

        bool IsContainer() const override final { return true; }

    protected:

        void OnPaint(CPaintEvent* paintEvent) override;

        void OnPaintOverlay(CPaintEvent* paintEvent) override;

        void HandleEvent(CEvent* event) override;


        b8 isVerticalScrollPressed = false;
        b8 isVerticalScrollHovered = false;
    };
    
} // namespace CE::Widgets

#include "CScrollArea.rtti.h"