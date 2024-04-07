#pragma once

namespace CE::Widgets
{
    CLASS()
    class CRYSTALWIDGETS_API CPopup : public CWindow, public ApplicationMessageHandler
    {
        CE_CLASS(CPopup, CWindow)
    public:

        CPopup();
        virtual ~CPopup();

        bool IsLayoutCalculationRoot() override final { return true; }

        void Show() override;

        void Hide() override;

        void Show(Vec2i screenPosition, Vec2i size);

    protected:

        void OnWindowDestroyed(PlatformWindow* window) override;

        void HandleEvent(CEvent* event) override;

        void OnBeforeDestroy() override;

        
        Vec2i showPosition = Vec2i(0, 0);

        Vec2i showSize = Vec2i(0, 0);

    };
    
} // namespace CE::Widgets

#include "CPopup.rtti.h"