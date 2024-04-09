#pragma once

namespace CE::Widgets
{
    CLASS()
    class CRYSTALWIDGETS_API CMenu : public CPopup
    {
        CE_CLASS(CMenu, CPopup)
    public:

        CMenu();
        virtual ~CMenu();

        void Show() override;

        void Hide() override;

        void Show(Vec2i screenPosition, Vec2i size);

    protected:

        void OnPlatformWindowSet() override;

        void HandleEvent(CEvent* event) override;

        void OnSubobjectAttached(Object* subobject) override;
        void OnSubobjectDetached(Object* subobject) override;

        bool WindowHitTest(PlatformWindow* window, Vec2 position);

        FIELD()
        Array<CMenuItem*> menuItems{};

    };
    
} // namespace CE::Widgets

#include "CMenu.rtti.h"