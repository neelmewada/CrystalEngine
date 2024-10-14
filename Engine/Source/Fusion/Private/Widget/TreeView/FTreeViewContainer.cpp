#include "Fusion.h"

namespace CE
{

    FTreeViewContainer::FTreeViewContainer()
    {

    }

    void FTreeViewContainer::Construct()
    {
        Super::Construct();
        
    }

    void FTreeViewContainer::SetContextRecursively(FFusionContext* context)
    {
	    Super::SetContextRecursively(context);

    }

    FWidget* FTreeViewContainer::HitTest(Vec2 localMousePos)
    {
	    return Super::HitTest(localMousePos);
    }

    bool FTreeViewContainer::ChildExistsRecursive(FWidget* child)
    {
        if (this == child)
            return true;

        for (FTreeViewRow* widget : children)
        {
            if (widget->ChildExistsRecursive(child))
                return true;
        }

        return false;
    }

    void FTreeViewContainer::ApplyStyleRecursively()
    {
        Super::ApplyStyleRecursively();

        for (FTreeViewRow* widget : children)
        {
            widget->ApplyStyleRecursively();
        }
    }

    void FTreeViewContainer::HandleEvent(FEvent* event)
    {
        Super::HandleEvent(event);
    }

    void FTreeViewContainer::CalculateIntrinsicSize()
    {
        Super::CalculateIntrinsicSize();

    }

    void FTreeViewContainer::PlaceSubWidgets()
    {
        Super::PlaceSubWidgets();

    }
    
}

