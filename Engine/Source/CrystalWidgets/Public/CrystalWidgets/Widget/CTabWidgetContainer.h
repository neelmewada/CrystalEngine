#pragma once

namespace CE::Widgets
{
    CLASS()
    class CRYSTALWIDGETS_API CTabWidgetContainer : public CWidget
    {
        CE_CLASS(CTabWidgetContainer, CWidget)
    public:

        CTabWidgetContainer();
        virtual ~CTabWidgetContainer();

        const String& GetTitle() const { return title; }

        void SetTitle(const String& title) { this->title = title; }

    protected:

        FIELD()
        String title{};

    };
    
} // namespace CE::Widgets

#include "CTabWidgetContainer.rtti.h"