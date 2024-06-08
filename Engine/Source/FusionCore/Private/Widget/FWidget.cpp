#include "FusionCore.h"

namespace CE
{

    FWidget::FWidget()
    {
        
    }

    FWidget::~FWidget()
    {
        
    }

    FFusionContext* FWidget::GetContext()
    {
        if (!context && parent)
        {
            context = parent->GetContext();
        }
        return context;
    }

    void FWidget::OnAfterConstruct()
    {
	    Super::OnAfterConstruct();

        Construct();
    }

    void FWidget::OnBeforeDestroy()
    {
	    Super::OnBeforeDestroy();

        if (parent)
        {
            parent->OnChildWidgetDestroyed(this);
        }
    }

    void FWidget::PrecomputeIntrinsicSize()
    {

    }

    void FWidget::CalculateLayout(Vec2 availableSize)
    {
        
    }

    void FWidget::AddChild(FWidget* child)
    {
        if (TryAddChild(child))
        {
            child->parent = this;
        }
    }

    void FWidget::Construct()
    {
        
    }

} // namespace CE

