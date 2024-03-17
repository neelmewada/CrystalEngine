#pragma once

namespace CE::Widgets
{
    class CWindow;

    CLASS()
    class CRYSTALWIDGETS_API CWidget : public Object
    {
        CE_CLASS(CWidget, Object)
    public:

        CWidget();
        virtual ~CWidget();

    protected:

        FIELD()
        Array<CWidget*> attachedWidgets{};


    };

} // namespace CE::Widgets

#include "CWidget.rtti.h"
