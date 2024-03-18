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


    crystalwidgets_protected_internal:

        FIELD()
        Array<CWidget*> attachedWidgets{};

        FIELD()
        CWidget* parent = nullptr;

        FIELD()
        CWindow* ownerWindow = nullptr;



    crystalwidgets_internal:

        YGNodeRef node = nullptr;

    };

} // namespace CE::Widgets

#include "CWidget.rtti.h"
