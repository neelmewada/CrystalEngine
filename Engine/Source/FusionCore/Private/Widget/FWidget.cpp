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

        if (GetContext())
        {
            GetContext()->OnWidgetDestroyed(this);
        }

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

        intrinsicSize.width = Math::Clamp(intrinsicSize.width,
            m_MinWidth + m_Padding.left + m_Padding.right,
            m_MaxWidth + m_Padding.left + m_Padding.right);

        intrinsicSize.height = Math::Clamp(intrinsicSize.height,
            m_MinHeight + m_Padding.top + m_Padding.bottom,
            m_MaxHeight + m_Padding.top + m_Padding.bottom);
    }

    void FWidget::PlaceSubWidgets()
    {
        localTransform = Matrix4x4::Translation(computedPosition + m_Translation) * 
            Matrix4x4::Angle(m_Rotation) * 
            Matrix4x4::Scale(m_Scale);
    }

    void FWidget::AddChild(FWidget* child)
    {
        if (TryAddChild(child))
        {
            child->parent = this;
            child->OnAttachedToParent(this);
            MarkLayoutDirty();
        }
    }

    void FWidget::RemoveChild(FWidget* child)
    {
        if (TryRemoveChild(child))
        {
            child->OnDetachedFromParent(this);
            child->parent = nullptr;
            MarkLayoutDirty();
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

    void FWidget::MarkDirty()
    {
        FFusionContext* context = GetContext();
        if (context)
        {
            context->MarkDirty();
        }
    }

    void FWidget::Construct()
    {
        
    }

} // namespace CE

