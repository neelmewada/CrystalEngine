#include "FusionCore.h"

namespace CE
{

    FContainerWidget::FContainerWidget()
    {
	    
    }

    bool FContainerWidget::TryAddChild(FWidget* child)
    {
        if (children.Exists(child))
            return false;

        children.Add(child);
        return true;
    }

    void FContainerWidget::OnChildWidgetDestroyed(FWidget* child)
    {
	    Super::OnChildWidgetDestroyed(child);

        children.Remove(child);
    }

} // namespace CE
