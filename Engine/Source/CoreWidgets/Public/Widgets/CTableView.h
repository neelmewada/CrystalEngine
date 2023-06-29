#pragma once

namespace CE::Widgets
{

    CLASS()
    class COREWIDGETS_API CTableView : public CWidget
    {
        CE_CLASS(CTableView, CWidget)
    public:
        CTableView();
        virtual ~CTableView();
    };
    
    
} // namespace CE::Widgets

#include "CWidget.rtti.h"
