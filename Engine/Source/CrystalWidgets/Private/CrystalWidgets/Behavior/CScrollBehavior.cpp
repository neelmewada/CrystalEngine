#include "CrystalWidgets.h"

namespace CE::Widgets
{

    CScrollBehavior::CScrollBehavior()
    {

    }

    CScrollBehavior::~CScrollBehavior()
    {

    }

    Rect CScrollBehavior::GetVerticalScrollBarRect()
    {
        if (self->allowVerticalScroll)
        {
            auto app = CApplication::Get();

            Vec2 originalPos = self->GetComputedLayoutTopLeft();
            Vec2 originalSize = self->GetComputedLayoutSize();
            f32 originalHeight = originalSize.height;
            f32 contentMaxY = self->contentSize.height;

            if (contentMaxY > originalHeight + app->styleConstants.scrollSizeBuffer)
            {
                Rect scrollRegion = Rect::FromSize(Vec2(originalSize.width - app->styleConstants.scrollRectWidth, 0) + originalPos,
                    Vec2(app->styleConstants.scrollRectWidth, originalHeight));
                f32 scrollRectHeightRatio = originalHeight / contentMaxY;

                Rect scrollRect = Rect::FromSize(scrollRegion.min,
                    Vec2(scrollRegion.GetSize().width, 
                        Math::Max(scrollRegion.GetSize().height * scrollRectHeightRatio, app->styleConstants.minScrollRectSize)));
                scrollRect = scrollRect.Translate(Vec2(0, (originalHeight - scrollRect.GetSize().height) * self->normalizedScroll.y));
                
                return scrollRect;
            }
        }

        return Rect();
    }

    void CScrollBehavior::OnPaintOverlay(CPainter* painter)
    {
        auto app = CApplication::Get();

        if (self->allowVerticalScroll) // Draw Vertical Scroll Bar
        {
            Vec2 originalSize = self->GetComputedLayoutSize();
            f32 originalHeight = originalSize.height;
            f32 contentMaxY = self->contentSize.height;

            if (contentMaxY > originalHeight + app->styleConstants.scrollSizeBuffer)
            {
                Rect scrollRect = GetVerticalScrollBarRect();

                CPen pen{};
                CBrush brush = CBrush(Color::RGBA(87, 87, 87));

                if (isVerticalScrollHovered || isVerticalScrollPressed)
                {
                    brush.SetColor(Color::RGBA(128, 128, 128));
                }

                painter->SetPen(pen);
                painter->SetBrush(brush);

                painter->DrawRoundedRect(scrollRect, Vec4(1, 1, 1, 1) * app->styleConstants.scrollRectWidth * 0.5f);
            }
            else
            {
                self->normalizedScroll = Vec2(0, 0);
            }
        }
    }

    bool CScrollBehavior::IsVerticalScrollVisible()
    {
        auto app = CApplication::Get();

        Vec2 originalSize = self->GetComputedLayoutSize();
        f32 originalHeight = originalSize.height;
        f32 contentMaxY = self->contentSize.height;

        return contentMaxY > originalHeight + app->styleConstants.scrollSizeBuffer;
    }

    void CScrollBehavior::HandleEvent(CEvent* event)
    {
        if (self->allowVerticalScroll || self->allowHorizontalScroll)
            self->receiveDragEvents = true;

        auto app = CApplication::Get();
        
        if (event->IsMouseEvent())
        {
            CMouseEvent* mouseEvent = static_cast<CMouseEvent*>(event);
            Vec2 globalMousePos = mouseEvent->mousePos;
            Rect screenSpaceWindowRect = self->GetScreenSpaceRect();
            Vec2 windowSpaceMousePos = globalMousePos - screenSpaceWindowRect.min;
            Vec2 mouseDelta = mouseEvent->mousePos - mouseEvent->prevMousePos;

            if (mouseEvent->type == CEventType::MouseMove && (self->allowVerticalScroll || self->allowHorizontalScroll))
            {
                isVerticalScrollHovered = false;
                self->SetNeedsPaint();

                if (self->allowVerticalScroll)
                {
                    Vec2 originalSize = self->GetComputedLayoutSize();
                    f32 originalHeight = originalSize.height;
                    f32 contentMaxY = self->contentSize.height;

                    if (contentMaxY > originalHeight + app->styleConstants.scrollSizeBuffer)
                    {
                        Rect scrollRect = GetVerticalScrollBarRect();
                        scrollRect.min -= self->GetComputedLayoutTopLeft();
                        scrollRect.max -= self->GetComputedLayoutTopLeft();
                        scrollRect = self->LocalToScreenSpaceRect(scrollRect);

                        if (scrollRect.Contains(globalMousePos))
                        {
                            isVerticalScrollHovered = true;
                        }
                    }
                }
            }
            else if (mouseEvent->type == CEventType::MouseLeave && (self->allowVerticalScroll || self->allowHorizontalScroll))
            {
                isVerticalScrollHovered = false;
                self->SetNeedsPaint();
            }
            else if (mouseEvent->type == CEventType::DragBegin && (self->allowVerticalScroll || self->allowHorizontalScroll))
            {
                CDragEvent* dragEvent = (CDragEvent*)event;

                if (self->allowVerticalScroll)
                {
                    Vec2 originalSize = self->GetComputedLayoutSize();
                    f32 originalHeight = originalSize.height;
                    f32 contentMaxY = self->contentSize.height;

                    if (contentMaxY > originalHeight + app->styleConstants.scrollSizeBuffer)
                    {
                        Rect scrollRect = GetVerticalScrollBarRect();
                        scrollRect.min -= self->GetComputedLayoutTopLeft();
                        scrollRect.max -= self->GetComputedLayoutTopLeft();
                        scrollRect = self->LocalToScreenSpaceRect(scrollRect);

                        if (scrollRect.Contains(globalMousePos))
                        {
                            dragEvent->draggedWidget = self;
                            dragEvent->ConsumeAndStopPropagation(self);
                            isVerticalScrollPressed = true;
                        }
                    }
                }
            }
            else if (mouseEvent->type == CEventType::DragMove && isVerticalScrollPressed)
            {
                CDragEvent* dragEvent = (CDragEvent*)event;

                if (self->allowVerticalScroll && isVerticalScrollPressed)
                {
                    Vec2 originalSize = self->GetComputedLayoutSize();
                    f32 originalHeight = originalSize.height;

                    self->normalizedScroll.y += mouseDelta.y / (originalHeight - GetVerticalScrollBarRect().GetSize().height);
                    self->normalizedScroll.y = Math::Clamp01(self->normalizedScroll.y);

                    dragEvent->ConsumeAndStopPropagation(self);
                    self->SetNeedsLayout();
                    self->SetNeedsPaint();
                }
            }
            else if (mouseEvent->type == CEventType::DragEnd && isVerticalScrollPressed)
            {
                CDragEvent* dragEvent = (CDragEvent*)event;

                isVerticalScrollPressed = false;
                dragEvent->ConsumeAndStopPropagation(self);
            }
            else if (mouseEvent->type == CEventType::MouseWheel && self->allowVerticalScroll && event->sender != nullptr)
            {
                CWidget* sender = event->sender;

                while (sender != nullptr)
                {
                    CScrollBehavior* scrollBehavior = sender->GetBehavior<CScrollBehavior>();

                    if (scrollBehavior != nullptr && scrollBehavior->IsVerticalScrollVisible() && sender->allowVerticalScroll)
                    {
                        if (sender == self)
                        {
                            Vec2 originalSize = self->GetComputedLayoutSize();
                            f32 originalHeight = originalSize.height;
                            f32 contentMaxY = self->contentSize.height;

                            if (contentMaxY > originalHeight + app->styleConstants.scrollSizeBuffer) // If scrolling is possible
                            {
                                self->normalizedScroll.y += -mouseEvent->wheelDelta.y * self->scrollSensitivity / (contentMaxY - originalHeight);
                                self->normalizedScroll.y = Math::Clamp01(self->normalizedScroll.y);

                                self->SetNeedsLayout();
                                self->SetNeedsPaint();
                            }
                        }

	                    break;
                    }

                    sender = sender->parent;
                }
            }

            Super::HandleEvent(event);
        }

    }

    Vec4 CScrollBehavior::GetExtraRootPadding()
    {
        auto app = CApplication::Get();

	    return Super::GetExtraRootPadding();
    }

}
