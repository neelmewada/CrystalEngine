#pragma once

namespace CE::Widgets
{
    class CTabWidgetContainer;
    DECLARE_SCRIPT_EVENT(CTabSelectionEvent, int tabIndex);

    CLASS()
    class CRYSTALWIDGETS_API CTabWidget : public CWidget
    {
        CE_CLASS(CTabWidget, CWidget)
    public:

        CTabWidget();
        virtual ~CTabWidget();

        bool IsSubWidgetAllowed(Class* subWidgetClass) override;

        void SetActiveTab(int tabIndex);

        int GetActiveTabIndex() const;

        CTabWidgetContainer* GetActiveTab() const { return activeTab; }

        // - Events -

        FIELD()
        CTabSelectionEvent onTabSelectionChanged{};

    protected:

        void OnPaint(CPaintEvent* paintEvent) override;

        void OnSubobjectAttached(Object* subobject) override;
        void OnSubobjectDetached(Object* subobject) override;

        void HandleEvent(CEvent* event) override;

    private:

        FIELD()
        Array<CTabWidgetContainer*> containers{};

        FIELD()
        CTabWidgetContainer* activeTab = nullptr;

        Array<Rect> tabRects{};

        int hoveredTab = -1;
    };
    
} // namespace CE::Widgets

#include "CTabWidget.rtti.h"
