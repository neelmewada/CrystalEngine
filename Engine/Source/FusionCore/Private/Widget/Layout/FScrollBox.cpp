#include "FusionCore.h"

namespace CE
{

    FScrollBox::FScrollBox()
    {
        m_VerticalScroll = true;
        m_HorizontalScroll = false;
        m_ClipChildren = true;

        m_ScrollBarShape = FRoundedRectangle(1.5f);
        m_ScrollBarWidth = 10;
        m_ScrollBarMargin = 2;
    }

    void FScrollBox::CalculateIntrinsicSize()
    {
        intrinsicSize = Vec2(m_MinWidth + m_Padding.left + m_Padding.right,
            m_MinHeight + m_Padding.top + m_Padding.bottom);

        FWidget* child = GetChild();

        if (!child || !child->Enabled())
            return;

        child->CalculateIntrinsicSize();

        Vec2 childSize = child->GetIntrinsicSize();
        const Vec4& childMargin = child->Margin();

        intrinsicSize.width = Math::Max(intrinsicSize.width, childSize.width + m_Padding.left + m_Padding.right + childMargin.left + childMargin.right);
        intrinsicSize.height = Math::Max(intrinsicSize.height, childSize.height + m_Padding.top + m_Padding.bottom + childMargin.top + childMargin.bottom);

        ApplyIntrinsicSizeConstraints();
    }

    void FScrollBox::PlaceSubWidgets()
    {
        ApplySizeConstraints();
        UpdateLocalTransform();

        FWidget* child = GetChild();

        if (!child || !child->Enabled())
            return;

        Vec4 childMargin = child->Margin();
        Vec2 childIntrinsicSize = child->GetIntrinsicSize();
        Vec2 availableSize = computedSize - Vec2(m_Padding.left + m_Padding.right + childMargin.left + childMargin.right,
            m_Padding.top + m_Padding.bottom + childMargin.top + childMargin.bottom);

        if (isVerticalScrollVisible)
        {
            availableSize.x -= m_ScrollBarMargin * 2 + m_ScrollBarWidth;
        }
        if (isHorizontalScrollVisible)
        {
            availableSize.y -= m_ScrollBarMargin * 2 + m_ScrollBarWidth;
        }

        child->computedPosition = Vec2(m_Padding.left + childMargin.left, m_Padding.top + childMargin.top);
        child->computedSize = computedSize -
            Vec2(m_Padding.left + m_Padding.right + childMargin.left + childMargin.right,
                m_Padding.top + m_Padding.bottom + childMargin.top + childMargin.bottom);

        CE::VAlign childVAlign = child->VAlign();
        CE::HAlign childHAlign = child->HAlign();

        switch (childVAlign)
        {
        case VAlign::Auto:
        case VAlign::Fill:
        case VAlign::Top:
            child->computedSize.height = childIntrinsicSize.height;
            child->computedPosition.y = m_Padding.top + childMargin.top;
            break;
        case VAlign::Center:
            child->computedSize.height = childIntrinsicSize.height;
            child->computedPosition.y = m_Padding.top + childMargin.top + (availableSize.height - childIntrinsicSize.height) * 0.5f;
            break;
        case VAlign::Bottom:
            child->computedSize.height = childIntrinsicSize.height;
            child->computedPosition.y = m_Padding.top + childMargin.top + (availableSize.height - childIntrinsicSize.height);
            break;
        }
        
        switch (childHAlign)
        {
        case HAlign::Auto:
        case HAlign::Fill:
        case HAlign::Left:
            child->computedSize.width = childIntrinsicSize.width;
            child->computedPosition.x = m_Padding.left + childMargin.left;
            break;
        case HAlign::Center:
            child->computedSize.width = childIntrinsicSize.width;
            child->computedPosition.x = m_Padding.left + childMargin.left + (availableSize.width - childIntrinsicSize.width) * 0.5f;
            break;
        case HAlign::Right:
            child->computedSize.width = childIntrinsicSize.width;
            child->computedPosition.x = m_Padding.left + childMargin.left + (availableSize.width - childIntrinsicSize.width);
            break;
        }

        child->PlaceSubWidgets();
        
        if (child->computedSize.height > availableSize.height && VerticalScroll())
        {
	        if (!isVerticalScrollVisible)
	        {
                isVerticalScrollVisible = true;
                MarkLayoutDirty();
	        }
        }
        else if (isVerticalScrollVisible)
        {
            isVerticalScrollVisible = false;
            MarkLayoutDirty();
        }

        if (child->computedSize.width > availableSize.width && HorizontalScroll())
        {
	        if (!isHorizontalScrollVisible)
	        {
                isHorizontalScrollVisible = true;
                MarkLayoutDirty();
	        }
        }
        else if (isHorizontalScrollVisible)
        {
            isHorizontalScrollVisible = false;
            MarkLayoutDirty();
        }
    }

    FWidget* FScrollBox::HitTest(Vec2 mousePosition)
    {
        FWidget* hitResult = Super::HitTest(mousePosition);
        if (hitResult == nullptr)
            return nullptr;

        
        return hitResult;
    }

    void FScrollBox::OnPaint(FPainter* painter)
    {
	    Super::OnPaint(painter);

        // TODO: Paint scroll bars

        if (isVerticalScrollVisible)
        {
            if (m_ScrollBarBackground.IsValidBrush() || m_ScrollBarBackgroundPen.IsValidPen())
            {
                painter->SetBrush(m_ScrollBarBackground);
                painter->SetPen(m_ScrollBarBackgroundPen);

                Vec2 pos = Vec2(computedPosition.x + computedSize.x - m_ScrollBarMargin * 2 - m_ScrollBarWidth, computedPosition.y);
                Vec2 size = Vec2(m_ScrollBarMargin * 2 + m_ScrollBarWidth, computedSize.y);

                painter->DrawRect(Rect::FromSize(pos, size));
            }

            if (m_ScrollBarBrush.IsValidBrush())
            {
                painter->SetBrush(m_ScrollBarBrush);
                painter->SetPen(m_ScrollBarPen);


            }
        }

        if (isHorizontalScrollVisible)
        {
            if (m_ScrollBarBackground.IsValidBrush() || m_ScrollBarBackgroundPen.IsValidPen())
            {
                painter->SetBrush(m_ScrollBarBackground);
                painter->SetPen(m_ScrollBarBackgroundPen);

                f32 horiOffset = 0;
                if (isVerticalScrollVisible)
                    horiOffset = m_ScrollBarMargin * 2 + m_ScrollBarWidth;

                Vec2 pos = Vec2(computedPosition.x, computedPosition.y + computedSize.y - m_ScrollBarMargin * 2 - m_ScrollBarWidth);
                Vec2 size = Vec2(computedSize.x - horiOffset, m_ScrollBarMargin * 2 + m_ScrollBarWidth);

                painter->DrawRect(Rect::FromSize(pos, size));
            }

            if (m_ScrollBarBrush.IsValidBrush())
            {
                painter->SetBrush(m_ScrollBarBrush);
                painter->SetPen(m_ScrollBarPen);


            }
        }
    }

    void FScrollBox::HandleEvent(FEvent* event)
    {
	    Super::HandleEvent(event);
    }

}

