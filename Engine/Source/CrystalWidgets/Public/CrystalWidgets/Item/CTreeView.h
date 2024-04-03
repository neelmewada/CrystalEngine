#pragma once

namespace CE::Widgets
{
    CLASS()
    class CRYSTALWIDGETS_API CTreeView : public CBaseItemView
    {
        CE_CLASS(CTreeView, CBaseItemView)
    public:

        CTreeView();
        virtual ~CTreeView();

    protected:

        void OnPaint(CPaintEvent* paintEvent) override;

        void OnPaintOverlay(CPaintEvent* paintEvent) override;

    private:

    };
    
} // namespace CE::Widgets

#include "CTreeView.rtti.h"