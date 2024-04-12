#pragma once

namespace CE::Widgets
{
    CLASS()
    class CRYSTALWIDGETS_API CToolWindow : public CWindow
    {
        CE_CLASS(CToolWindow, CWindow)
    public:

        CToolWindow();
        virtual ~CToolWindow();

    protected:

        void Construct() override;

        void OnPlatformWindowSet() override;

        void OnBeforeDestroy() override;

        void OnPaint(CPaintEvent* paintEvent) override;

        void HandleEvent(CEvent* event) override;

    };
    
} // namespace CE::Widgets

#include "CToolWindow.rtti.h"