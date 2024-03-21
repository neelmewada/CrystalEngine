#include "CrystalWidgets.h"

namespace CE::Widgets
{
    CDockSplitView::CDockSplitView()
    {

    }

    CDockSplitView::~CDockSplitView()
    {

    }

    CDockSplitView* CDockSplitView::FindSplit(Name splitName)
    {
        if (GetName() == splitName)
            return this;

        for (auto child : children)
        {
            CDockSplitView* result = child->FindSplit(splitName);
            if (result)
            {
                return result;
            }
        }

        return nullptr;
    }

    void CDockSplitView::OnSubobjectAttached(Object* subobject)
    {
        Super::OnSubobjectAttached(subobject);

        if (subobject->IsOfType<CDockSplitView>())
        {
            CDockSplitView* splitView = (CDockSplitView*)subobject;
            splitView->dockSpace = dockSpace;
            children.Add(splitView);
            splitView->parentSplitView = this;
            rootPadding = Vec4(0, 0, 0, 0);
        }
        else if (subobject->IsOfType<CDockWindow>())
        {
            CDockWindow* dockWindow = (CDockWindow*)subobject;
            dockWindow->dockSpace = dockSpace;
            if (dockSpace->GetDockType() == CDockType::Major)
                rootPadding = Vec4(0, 60, 0, 0);
            else
                rootPadding = Vec4(0, 30, 0, 0);
        }
    }

    void CDockSplitView::OnSubobjectDetached(Object* subobject)
    {
        Super::OnSubobjectDetached(subobject);

        if (subobject->IsOfType<CDockSplitView>())
        {
            CDockSplitView* splitView = (CDockSplitView*)subobject;
            splitView->dockSpace = nullptr;
            children.Remove(splitView);
            splitView->parentSplitView = nullptr;
        }
        else if (subobject->IsOfType<CDockWindow>())
        {
            CDockWindow* dockWindow = (CDockWindow*)subobject;
            dockWindow->dockSpace = nullptr;
        }
    }
} // namespace CE::Widgets
