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
        parent = nullptr;
    }

    void FWidget::PrecomputeIntrinsicSize()
    {
        intrinsicSize = Vec2(m_MinWidth + m_Padding.left + m_Padding.right, 
            m_MinHeight + m_Padding.top + m_Padding.bottom);
    }

    void FWidget::PlaceSubWidgets()
    {
        
    }

    void FWidget::AddChild(FWidget* child)
    {
        if (TryAddChild(child))
        {
            child->parent = this;
        }
    }

    void FWidget::MarkLayoutDirty()
    {
        FFusionContext* context = GetContext();
        if (context)
        {
            context->MarkLayoutDirty();
        }
    }

    void FWidget::Construct()
    {
        
    }

} // namespace CE

