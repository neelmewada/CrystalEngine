#pragma once

namespace CE::Widgets
{

    CLASS()
    class CRYSTALWIDGETS_API CGameWindow : public CWindow
    {
        CE_CLASS(CGameWindow, CWindow)
    public:
        CGameWindow();

        virtual ~CGameWindow();

    protected:

        void Construct() override;

        void OnPaint(CPaintEvent* paintEvent) override;

        void OnPaintOverlay(CPaintEvent* paintEvent) override;

    };
    
} // namespace CE::Widgets

#include "CGameWindow.rtti.h"