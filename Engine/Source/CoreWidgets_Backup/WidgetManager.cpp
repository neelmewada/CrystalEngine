
#include "CoreWidgets.h"

namespace CE::Widgets
{

    WidgetManager::WidgetManager()
    {
        
    }

    WidgetManager::~WidgetManager()
    {
        
    }

    void WidgetManager::Tick()
    {
        for (CWidget* widget : widgets)
        {
            widget->RenderGUI();
        }
    }

    void WidgetManager::AddWidget(CWidget *widget)
    {
        if (widget == nullptr)
            return;
        widgets.Add(widget);
        AttachSubobject(widget);
    }

    void WidgetManager::RemoveWidget(CWidget *widget)
    {
        if (widget == nullptr)
            return;
        DetachSubobject(widget);
        widgets.Remove(widget);
    }

    
} // namespace CE::Widgets

