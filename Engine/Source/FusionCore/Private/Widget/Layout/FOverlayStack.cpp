#include "FusionCore.h"

namespace CE
{

    FOverlayStack::FOverlayStack()
    {

    }

    void FOverlayStack::CalculateIntrinsicSize()
    {
        ZoneScoped;

        if (children.IsEmpty())
        {
            Super::CalculateIntrinsicSize();
            return;
        }

        intrinsicSize.width = m_Padding.left + m_Padding.right;
        intrinsicSize.height = m_Padding.top + m_Padding.bottom;

        Vec2 contentSize = {};

        for (FWidget* child : children)
        {
            child->CalculateIntrinsicSize();

            Vec2 childSize = child->GetIntrinsicSize();
            Vec4 childMargin = child->Margin();

            contentSize.width = Math::Max(contentSize.width, childSize.width + childMargin.left + childMargin.right);
            contentSize.height = Math::Max(contentSize.height, childSize.height + childMargin.top + childMargin.bottom);
        }

        intrinsicSize += contentSize;

        intrinsicSize.width = Math::Clamp(intrinsicSize.width,
            m_MinWidth + m_Padding.left + m_Padding.right,
            m_MaxWidth + m_Padding.left + m_Padding.right);

        intrinsicSize.height = Math::Clamp(intrinsicSize.height,
            m_MinHeight + m_Padding.top + m_Padding.bottom,
            m_MaxHeight + m_Padding.top + m_Padding.bottom);
    }

    void FOverlayStack::PlaceSubWidgets()
    {
        ZoneScoped;

	    Super::PlaceSubWidgets();

        if (children.IsEmpty())
        {
            return;
        }

        Vec2 origin = Vec2(m_Padding.left, m_Padding.top);
        f32 crossAxisSize = 0;
        f32 remainingSize = 0;
        Vec2 availableSize = computedSize - Vec2(m_Padding.left + m_Padding.right,
            m_Padding.top + m_Padding.bottom);

        for (FWidget* child : children)
        {
            if (!child)
                continue;

            CE::VAlign childVAlign = child->VAlign();
            CE::HAlign childHAlign = child->HAlign();

            if (childVAlign == VAlign::Auto)
                childVAlign = m_ContentVAlign;
            if (childHAlign == HAlign::Auto)
                childHAlign = m_ContentHAlign;

            Vec2 childPos;
            Vec2 childSize;
            Vec2 childIntrinsicSize = child->GetIntrinsicSize();

            switch (childVAlign)
            {
            case VAlign::Auto:
            case VAlign::Fill:
                childSize.y = availableSize.y;
                childPos.y = origin.y;
	            break;
            case VAlign::Top:
                childSize.y = childIntrinsicSize.y;
                childPos.y = origin.y;
	            break;
            case VAlign::Center:
                childSize.y = childIntrinsicSize.y;
                childPos.y = origin.y + (availableSize.y - childSize.y) * 0.5f;
	            break;
            case VAlign::Bottom:
                childSize.y = childIntrinsicSize.y;
                childPos.y = origin.y + (availableSize.y - childSize.y);
	            break;
            }

            switch (childHAlign)
            {
            case HAlign::Auto:
            case HAlign::Fill:
                childSize.x = availableSize.x;
                childPos.x = origin.x;
	            break;
            case HAlign::Left:
                childSize.x = childIntrinsicSize.x;
                childPos.x = origin.x;
	            break;
            case HAlign::Center:
                childSize.x = childIntrinsicSize.x;
                childPos.x = origin.x + (availableSize.x - childSize.x) * 0.5f;
	            break;
            case HAlign::Right:
                childSize.x = childIntrinsicSize.x;
                childPos.x = origin.x + (availableSize.x - childSize.x);
	            break;
            }

            childPos.x = Math::Max(childPos.x, origin.x);
            childPos.y = Math::Max(childPos.y, origin.y);
            
            child->SetComputedPosition(childPos);
            child->SetComputedSize(childSize);

            child->PlaceSubWidgets();
        }
    }

    void FOverlayStack::OnPaint(FPainter* painter)
    {
	    Super::OnPaint(painter);

    }

    void FOverlayStack::Construct()
    {
        
        
    }
    
}

