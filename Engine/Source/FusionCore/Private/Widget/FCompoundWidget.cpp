#include "FusionCore.h"

namespace CE
{

    FCompoundWidget::FCompoundWidget()
    {

    }

    void FCompoundWidget::PrecomputeIntrinsicSize()
    {
        intrinsicSize = Vec2(m_MinWidth + m_Padding.left + m_Padding.right,
            m_MinHeight + m_Padding.top + m_Padding.bottom);

        if (!m_Child)
            return;

        m_Child->PrecomputeIntrinsicSize();

        Vec2 childSize = m_Child->GetIntrinsicSize();
        const Vec4& childMargin = m_Child->GetMargin();

        intrinsicSize.width = Math::Max(intrinsicSize.width, childSize.width + m_Padding.left + m_Padding.right + childMargin.left + childMargin.right);
        intrinsicSize.height = Math::Max(intrinsicSize.height, childSize.height + m_Padding.top + m_Padding.bottom + childMargin.top + childMargin.bottom);
    }

    void FCompoundWidget::PlaceSubWidgets()
    {
        Super::PlaceSubWidgets();

        if (!m_Child)
            return;

        Vec4 childMargin = m_Child->GetMargin();

        m_Child->computedPosition = Vec2(m_Padding.left + childMargin.left, m_Padding.top + childMargin.top);
        m_Child->computedSize = computedSize - 
            Vec2(m_Padding.left + m_Padding.right + childMargin.left + childMargin.right, 
                m_Padding.top + m_Padding.bottom + childMargin.top + childMargin.bottom);

        //m_Child->globalComputedPosition = globalComputedPosition + m_Child->computedPosition;

        m_Child->PlaceSubWidgets();
    }

    bool FCompoundWidget::TryAddChild(FWidget* child)
    {
        if (!child)
            return false;

        m_Child = child;
        child->parent = this;
        MarkLayoutDirty();
        return true;
    }

    bool FCompoundWidget::TryRemoveChild(FWidget* child)
    {
	    if (!child || child != m_Child)
            return false;

        m_Child = nullptr;
        child->parent = nullptr;
        MarkLayoutDirty();
        return true;
    }

    void FCompoundWidget::OnChildWidgetDestroyed(FWidget* child)
    {
	    Super::OnChildWidgetDestroyed(child);

        if (child == m_Child)
        {
            TryRemoveChild(child);
        }
    }

}

