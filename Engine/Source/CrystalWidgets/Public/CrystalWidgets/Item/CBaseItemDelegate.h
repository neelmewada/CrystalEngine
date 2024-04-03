#pragma once

namespace CE::Widgets
{
    CLASS(Abstract)
    class CRYSTALWIDGETS_API CBaseItemDelegate : public Object
    {
        CE_CLASS(CBaseItemDelegate, Object)
    public:

        CBaseItemDelegate();
        virtual ~CBaseItemDelegate();



    protected:

    };
    
} // namespace CE::Widgets

#include "CBaseItemDelegate.rtti.h"