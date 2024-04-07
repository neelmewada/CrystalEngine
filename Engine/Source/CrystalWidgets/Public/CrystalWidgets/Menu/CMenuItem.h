#pragma once

namespace CE::Widgets
{
    class CMenu;

    CLASS()
    class CRYSTALWIDGETS_API CMenuItem : public CWidget
    {
        CE_CLASS(CMenuItem, CWidget)
    public:

        CMenuItem();
        virtual ~CMenuItem();

        void SetIcon(Name iconPath);

        void SetText(const String& text);

        Name GetIcon() const { return icon->GetBackgroundImage(); }

        const String& GetText() const { return label->GetText(); }

        CMenu* GetSubMenu() const { return subMenu; }

        bool IsSubWidgetAllowed(Class* subWidgetClass) override;

    protected:

        void OnSubobjectAttached(Object* subobject) override;

        void OnSubobjectDetached(Object* subobject) override;

        FIELD()
        CMenu* subMenu = nullptr;

        FIELD()
        CWidget* icon = nullptr;

        FIELD()
        CLabel* label = nullptr;

    };
    
} // namespace CE::Widgets

#include "CMenuItem.rtti.h"