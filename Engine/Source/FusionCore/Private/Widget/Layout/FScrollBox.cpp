#include "FusionCore.h"

namespace CE
{
    constexpr f32 MinScrollBarHeight = 12.5f;

    FScrollBox::FScrollBox()
    {
        m_VerticalScroll = true;
        m_HorizontalScroll = false;
        m_ClipChildren = true;

        m_ScrollBarWidth = 8;
        m_ScrollBarShape = FRoundedRectangle(4);
        m_ScrollBarMargin = 2.5f;
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

    void FScrollBox::OnPaint(FPainter* painter)
    {
	    Super::OnPaint(painter);

        if (!GetChild())
            return;

        FWidget* child = GetChild();

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

                f32 scrollBarHeight = computedSize.y / child->computedSize.y;
                scrollBarHeight = Math::Max(scrollBarHeight, MinScrollBarHeight);
                f32 normalizedScrollY = NormalizedScrollY();
                
                Vec2 barPos = Vec2(computedPosition.x + computedSize.x - m_ScrollBarMargin - m_ScrollBarWidth,
                    computedPosition.y + normalizedScrollY * (computedSize.y - scrollBarHeight));
                
                painter->DrawShape(Rect::FromSize(barPos, Vec2(m_ScrollBarWidth, scrollBarHeight)), m_ScrollBarShape);
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
        FWidget* child = GetChild();

        if (event->IsMouseEvent() && event->sender == this && child)
        {
            FMouseEvent* mouseEvent = (FMouseEvent*)event;
            Vec2 localMousePos = mouseEvent->mousePosition;
            if (parent)
                localMousePos -= parent->globalPosition;

            bool isVScroll = false;

            if (isVerticalScrollVisible)
            {
                f32 scrollBarHeight = computedSize.y / child->computedSize.y;
                scrollBarHeight = Math::Max(scrollBarHeight, MinScrollBarHeight);
                f32 normalizedScrollY = NormalizedScrollY();

                Vec2 barPos = Vec2(computedPosition.x + computedSize.x - m_ScrollBarMargin - m_ScrollBarWidth,
                    computedPosition.y + normalizedScrollY * (computedSize.y - scrollBarHeight));
                Vec2 barSize = Vec2(m_ScrollBarWidth, scrollBarHeight);

                if (Rect::FromSize(barPos, barSize).Contains(localMousePos))
                {
                    isVScroll = true;
                }
            }

            if (mouseEvent->type == FEventType::MouseEnter || mouseEvent->type == FEventType::MouseMove || mouseEvent->type == FEventType::MouseLeave)
            {
	            if (isVScroll != isVerticalScrollHighlighted)
	            {
                    isVerticalScrollHighlighted = isVScroll;
                    MarkDirty();
	            }
            }
            
            if (mouseEvent->type == FEventType::DragBegin)
            {
                FDragEvent* dragEvent = static_cast<FDragEvent*>(mouseEvent);

                if (isVScroll)
                {
                    dragEvent->draggedWidget = this;
                    event->Consume(this);

                    isVerticalScrollDragged = true;
                }
            }
        }

        if (event->IsDragEvent())
        {
            FDragEvent* dragEvent = static_cast<FDragEvent*>(event);
            Vec2 mouseDelta = dragEvent->mousePosition - dragEvent->prevMousePosition;

            if (dragEvent->type == FEventType::DragMove)
            {
                if (isVerticalScrollDragged)
                {
                    f32 scrollBarHeight = computedSize.y / child->computedSize.y;
                    scrollBarHeight = Math::Max(scrollBarHeight, MinScrollBarHeight);
                    f32 normalizedScrollY = NormalizedScrollY();

                    f32 originalHeight = computedSize.y;

                    normalizedScrollY += mouseDelta.y / (originalHeight - scrollBarHeight);
                    normalizedScrollY = Math::Clamp01(normalizedScrollY);

                    NormalizedScrollY(normalizedScrollY);

                    dragEvent->draggedWidget = this;
                    dragEvent->Consume(this);
                }
                else if (isHorizontalScrollDragged)
                {

                }
            }
            else if (dragEvent->type == FEventType::DragEnd)
            {
                if (isVerticalScrollDragged)
                {
                    isVerticalScrollDragged = false;

                    dragEvent->draggedWidget = this;
                    dragEvent->Consume(this);
                }
                else if (isHorizontalScrollDragged)
                {
                    isHorizontalScrollDragged = false;

                    dragEvent->draggedWidget = this;
                    dragEvent->Consume(this);
                }
            }
        }

	    Super::HandleEvent(event);
    }

    FWidget* FScrollBox::HitTest(Vec2 mousePosition)
    {
        FWidget* hitResult = Super::HitTest(mousePosition);
        if (hitResult == nullptr)
            return nullptr;

        FWidget* child = GetChild();
        if (isVerticalScrollDragged)
        {
            return this;
        }

        if (child && isVerticalScrollVisible)
        {
            f32 scrollBarHeight = computedSize.y / child->computedSize.y;
            scrollBarHeight = Math::Max(scrollBarHeight, MinScrollBarHeight);
            f32 normalizedScrollY = NormalizedScrollY();

            Vec2 barPos = Vec2(computedPosition.x + computedSize.x - m_ScrollBarMargin - m_ScrollBarWidth,
                computedPosition.y + normalizedScrollY * (computedSize.y - scrollBarHeight));
            Vec2 barSize = Vec2(m_ScrollBarWidth, scrollBarHeight);

            if (Rect::FromSize(Vec2(computedPosition.x + computedSize.x - m_ScrollBarMargin - m_ScrollBarWidth, computedPosition.y), barSize).Contains(mousePosition))
            {
                return this;
            }
        }

        return hitResult;
    }

    FScrollBox& FScrollBox::NormalizedScrollY(f32 value)
    {
        value = Math::Clamp01(value);

        FWidget* child = GetChild();
        f32 scrollBarSection = m_ScrollBarMargin * 2 + m_ScrollBarWidth;

        if (child && isVerticalScrollVisible)
        {
            Vec2 translation = child->Translation();
            if (isHorizontalScrollVisible)
            {
                translation.y = -value * (child->computedSize.y - (computedSize.y - scrollBarSection));
            }
            else
            {
                translation.y = -value * (child->computedSize.y - computedSize.y);
            }
            child->Translation(translation);
        }
        return *this;
    }

    f32 FScrollBox::NormalizedScrollY()
    {
        FWidget* child = GetChild();
        f32 scrollBarSection = m_ScrollBarMargin * 2 + m_ScrollBarWidth;

        if (child && isVerticalScrollVisible)
        {
            Vec2 translation = child->Translation();
            if (isHorizontalScrollVisible)
            {
                return Math::Clamp01(-translation.y / (child->computedSize.y - (computedSize.y - scrollBarSection)));
            }
            else
            {
                return Math::Clamp01(-translation.y / (child->computedSize.y - computedSize.y));
            }
        }
        return 0;
    }

}

