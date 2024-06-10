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

        MarkLayoutDirty();
        return true;
    }

    bool FContainerWidget::TryRemoveChild(FWidget* child)
    {
        if (!child)
            return false;

        int index = children.IndexOf(child);
        if (index < 0)
            return false;

        children.RemoveAt(index);

        MarkLayoutDirty();
    	return true;
    }

    void FContainerWidget::OnChildWidgetDestroyed(FWidget* child)
    {
	    Super::OnChildWidgetDestroyed(child);

        TryRemoveChild(child);
    }

} // namespace CE
