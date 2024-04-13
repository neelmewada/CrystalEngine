#pragma once

namespace CE::Widgets
{
    class CTabWidgetContainer;

    CLASS()
    class CRYSTALWIDGETS_API CTabWidget : public CWidget
    {
        CE_CLASS(CTabWidget, CWidget)
    public:

        CTabWidget();
        virtual ~CTabWidget();

        bool IsSubWidgetAllowed(Class* subWidgetClass) override;

        void SetActiveTab(int tabIndex);

    protected:

        void OnPaint(CPaintEvent* paintEvent) override;

        void OnSubobjectAttached(Object* subobject) override;
        void OnSubobjectDetached(Object* subobject) override;

        void HandleEvent(CEvent* event) override;

    crystalwidgets_internal:

        FIELD()
        Array<CTabWidgetContainer*> containers{};

        FIELD()
        CTabWidgetContainer* activeTab = nullptr;

        Array<Rect> tabRects{};

        int hoveredTab = -1;
    };
    
} // namespace CE::Widgets

#include "CTabWidget.rtti.h"