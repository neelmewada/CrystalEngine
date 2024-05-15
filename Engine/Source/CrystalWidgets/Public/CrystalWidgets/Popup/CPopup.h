#pragma once

namespace CE::Widgets
{
    CLASS()
    class CRYSTALWIDGETS_API CPopup : public CWindow
    {
        CE_CLASS(CPopup, CWindow)
    public:

        CPopup();
        virtual ~CPopup();

        bool IsLayoutCalculationRoot() override final { return true; }

        bool IsSubWidgetAllowed(Class* subWidgetClass) override;

        void Show() override;

        void Hide() override;

        void Show(Vec2i screenPosition, Vec2i size);

        void ShowCenteredScreen(Vec2i size);

        bool IsShown();

    protected:

        void OnFocusLost() override;

        bool WindowHitTest(PlatformWindow* window, Vec2 position) override;

        void HandleEvent(CEvent* event) override;

        void OnBeforeDestroy() override;

        
        Vec2i showPosition = Vec2i(0, 0);

        Vec2i showSize = Vec2i(0, 0);


        CE_WIDGET_FRIENDS();
    };
    
} // namespace CE::Widgets

#include "CPopup.rtti.h"