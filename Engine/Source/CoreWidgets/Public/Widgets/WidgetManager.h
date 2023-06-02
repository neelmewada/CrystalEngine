#pragma once

namespace CE::Widgets
{
    class CWidget;

    CLASS()
    class COREWIDGETS_API WidgetManager : public Object
    {
        CE_CLASS(WidgetManager, CE::Object)
    public:
        
        WidgetManager();
        
        virtual ~WidgetManager();
        
        virtual void Tick();
        
        void AddWidget(CWidget* widget);
        void RemoveWidget(CWidget* widget);
        
    private:
        
        FIELD(Hidden)
        Array<CWidget*> widgets{};
    };
    
} // namespace CE::Widgets

#include "WidgetManager.rtti.h"
