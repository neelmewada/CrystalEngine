#include "CrystalWidgets.h"

namespace CE::Widgets
{
    constexpr f32 ScrollRectWidth = 10.0f;
    constexpr f32 MinScrollRectSize = 20.0f;
    constexpr f32 ScrollSizeBuffer = 1.0f;
    
    CWindow::CWindow()
    {
        receiveMouseEvents = false;
        clipChildren = true;

        title = GetName().GetString();
    }

    CWindow::~CWindow()
    {
        if (nativeWindow)
        {
            delete nativeWindow; nativeWindow = nullptr;
        }
    }

    void CWindow::Show()
    {
	    if (nativeWindow)
	    {
            nativeWindow->Show();
	    }
    }

    void CWindow::Hide()
    {
	    if (nativeWindow)
	    {
            nativeWindow->Hide();
	    }
    }

    void CWindow::SetPlatformWindow(PlatformWindow* window)
    {
        this->nativeWindow = new CPlatformWindow(this, window);

        receiveMouseEvents = true;

        OnPlatformWindowSet();
    }

    void CWindow::UpdateLayoutIfNeeded()
    {
        Super::UpdateLayoutIfNeeded();

    }

    void CWindow::OnAfterUpdateLayout()
    {
	    Super::OnAfterUpdateLayout();

        Vec2 originalSize = GetComputedLayoutSize();
        f32 contentMaxY = originalSize.height;
        f32 contentMaxX = originalSize.width;

        for (CWidget* widget : attachedWidgets)
        {
            if (!widget->IsEnabled())
                continue;

            Vec2 pos = widget->GetComputedLayoutTopLeft();
            Vec2 size = widget->GetComputedLayoutSize();
            f32 maxY = pos.y + size.height;
            f32 maxX = pos.x + size.width;
            if (isnan(maxX))
                maxX = 0;
            if (isnan(maxY))
                maxY = 0;

            contentMaxY = Math::Max(contentMaxY, maxY);
            contentMaxX = Math::Max(contentMaxX, maxX);
        }

        contentSize = Vec2(contentMaxX, contentMaxY);
    }


    void CWindow::OnBeforeDestroy()
    {
        Super::OnBeforeDestroy();

        delete nativeWindow; nativeWindow = nullptr;

        if (!IsDefaultInstance())
        {
            CApplication::Get()->GetFrameScheduler()->WaitUntilIdle();
        }
    }

    void CWindow::Construct()
    {
        Super::Construct();

        this->title = GetName().GetString();
    }

    Vec2 CWindow::CalculateIntrinsicSize(f32 width, f32 height)
    {
        return Vec2(windowSize.x <= 0 ? YGUndefined : windowSize.x, (allowVerticalScroll || windowSize.y <= 0) ? YGUndefined : windowSize.y);
    }

    bool CWindow::IsDockSpace()
    {
        return IsOfType<CDockSpace>();
    }

    CPlatformWindow* CWindow::GetRootNativeWindow()
    {
        if (nativeWindow)
            return nativeWindow;

        for (CWindow* owner = ownerWindow; owner != nullptr; owner = owner->ownerWindow)
        {
            if (owner->nativeWindow != nullptr)
                return owner->nativeWindow;
        }

        return nullptr;
    }

    static Rect ScaleRect(const Rect& rect, Vec2 scale = Vec2(1, 1))
    {
        Vec2 size = rect.GetSize();
        Vec2 center = (rect.min + rect.max) * 0.5f;
        return Rect(center - size / 2.0f * scale, center + size / 2.0f * scale);
    }

    static Rect ScaleRect(const Rect& rect, f32 scale)
    {
        return ScaleRect(rect, Vec2(scale, scale));
    }

    static Rect TranslateRect(const Rect& rect, Vec2 translation = Vec2(0, 0))
    {
        Vec2 size = rect.GetSize();
        return Rect::FromSize(rect.min + translation, size);
    }

    void CWindow::OnPaint(CPaintEvent* paintEvent)
    {
        Super::OnPaint(paintEvent);

        CPainter* painter = paintEvent->painter;

        Color bgColor = computedStyle.properties[CStylePropertyType::Background].color;

        CPen pen = CPen(); pen.SetColor(Color(1, 1, 1, 1)); pen.SetWidth(2.0f);
        CBrush brush = CBrush();

        if (nativeWindow != nullptr && PlatformMisc::GetCurrentPlatform() != PlatformName::Mac) // The CWindow is a native window
        {
            painter->SetPen(pen);
            painter->SetBrush(brush);

            Vec2 size = GetComputedLayoutSize() - Vec2(rootPadding.x + rootPadding.z, rootPadding.y + rootPadding.w);
            Vec2 pos = GetComputedLayoutTopLeft() + rootPadding.min;
            constexpr f32 controlWidth = 40;
            
            controlRects.Resize(3);

            for (int i = 0; i < 3; i++)
            {
                controlRects[i] = Rect::FromSize(pos + Vec2(size.width - controlWidth * (3 - i), 0), 
                    Vec2(controlWidth, controlWidth));
                // Shrink the rect perpendicular to it's edges
                controlRects[i] = Rect::FromSize(controlRects[i].min + Vec2(controlWidth, controlWidth) * 0.35f, 
                    Vec2(controlWidth, controlWidth) * 0.3f);
            }

            if (canBeClosed)
            {
                if (!hoveredControls[2])
                {
                    pen.SetColor(Color::RGBA8(180, 180, 180));
                    painter->SetPen(pen);
                }
                else
                {
                    pen.SetColor(Color::RGBA8(255, 255, 255));
                    painter->SetPen(pen);
                }

                painter->DrawRoundedX(controlRects[2]);
            }

            pen.SetWidth(1.25f);

            if (canBeMaximized)
            {
                if (!hoveredControls[1])
                {
                    pen.SetColor(Color::RGBA8(200, 200, 200));
                    painter->SetPen(pen);
                }
                else
                {
                    pen.SetColor(Color::White());
                    painter->SetPen(pen);
                }

	            if (!nativeWindow->platformWindow->IsMaximized())
	            {
                    painter->DrawRect(ScaleRect(controlRects[1], 0.98f));
	            }
	            else
	            {
                    brush.SetColor(bgColor);
                    painter->SetBrush(brush);

                    auto rectSize = controlRects[1].GetSize();
                    painter->DrawRect(TranslateRect(ScaleRect(controlRects[1], 0.8f), Vec2(1, -1) * 2.0f));
                    painter->DrawRect(ScaleRect(controlRects[1], 0.8f));
	            }
            }

            brush.SetColor(Color::Clear());
            pen.SetColor(Color::Clear());
            pen.SetWidth(0.0f);

            if (!hoveredControls[0])
            {
                brush.SetColor(Color::RGBA8(180, 180, 180));
                painter->SetPen(pen);
                painter->SetBrush(brush);
            }
            else
            {
                brush.SetColor(Color::White());
                painter->SetPen(pen);
                painter->SetBrush(brush);
            }

            if (canBeMinimized)
            {
                Vec2 rectSize = controlRects[0].GetSize();
                painter->DrawRect(Rect::FromSize(Vec2(controlRects[0].min.x, controlRects[0].min.y + rectSize.y / 2 - 2.0f / 2.0f),
                    Vec2(rectSize.width, 2.0f)));
            }
        }
    }

    void CWindow::OnPaintOverlay(CPaintEvent* paintEvent)
    {
        Super::OnPaintOverlay(paintEvent);

        CPainter* painter = paintEvent->painter;

        if (allowVerticalScroll) // Draw Vertical Scroll Bar
        {
            Vec2 originalSize = GetComputedLayoutSize();
            f32 originalHeight = originalSize.height;
            f32 contentMaxY = contentSize.height;

            if (contentMaxY > originalHeight + ScrollSizeBuffer)
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

                painter->DrawRoundedRect(scrollRect, Vec4(1, 1, 1, 1) * ScrollRectWidth * 0.5f);
            }
            else
            {
                normalizedScroll = Vec2(0, 0);
            }
        }
    }

    Rect CWindow::GetVerticalScrollBarRect()
    {
        if (allowVerticalScroll)
        {
            Vec2 originalSize = GetComputedLayoutSize();
            f32 originalHeight = originalSize.height;
            f32 contentMaxY = contentSize.height;

            if (contentMaxY > originalHeight + ScrollSizeBuffer)
            {
                Rect scrollRegion = Rect::FromSize(Vec2(originalSize.width - ScrollRectWidth, 0), Vec2(ScrollRectWidth, originalHeight));
                f32 scrollRectHeightRatio = originalHeight / contentMaxY;

                Rect scrollRect = Rect::FromSize(scrollRegion.min,
                    Vec2(scrollRegion.GetSize().width, Math::Max(scrollRegion.GetSize().height * scrollRectHeightRatio, MinScrollRectSize)));
                scrollRect = scrollRect.Translate(Vec2(0, (originalHeight - scrollRect.GetSize().height) * normalizedScroll.y));

                return scrollRect;
            }
        }

        return Rect();
    }

    void CWindow::HandleEvent(CEvent* event)
    {
        if (allowVerticalScroll || allowHorizontalScroll)
            receiveDragEvents = true;

        if (event->IsMouseEvent())
        {
            CMouseEvent* mouseEvent = static_cast<CMouseEvent*>(event);
            Vec2 globalMousePos = mouseEvent->mousePos;
            Rect screenSpaceWindowRect = GetScreenSpaceRect();
            Vec2 windowSpaceMousePos = globalMousePos - screenSpaceWindowRect.min;
            Vec2 mouseDelta = mouseEvent->mousePos - mouseEvent->prevMousePos;

            if (mouseEvent->type == CEventType::MouseMove && (allowVerticalScroll || allowHorizontalScroll))
            {
                isVerticalScrollHovered = false;
                SetNeedsPaint();

	            if (allowVerticalScroll)
	            {
                    Vec2 originalSize = GetComputedLayoutSize();
                    f32 originalHeight = originalSize.height;
                    f32 contentMaxY = contentSize.height;

                    if (contentMaxY > originalHeight + ScrollSizeBuffer)
                    {
                        Rect scrollRect = GetVerticalScrollBarRect();
                        scrollRect = LocalToScreenSpaceRect(scrollRect);

                        if (scrollRect.Contains(globalMousePos))
                        {
                            isVerticalScrollHovered = true;
                        }
                    }
	            }
            }
            else if (mouseEvent->type == CEventType::MouseLeave && (allowVerticalScroll || allowHorizontalScroll))
            {
                isVerticalScrollHovered = false;
                SetNeedsPaint();
            }
            else if (mouseEvent->type == CEventType::DragBegin && (allowVerticalScroll || allowHorizontalScroll))
            {
                CDragEvent* dragEvent = (CDragEvent*)event;

	            if (allowVerticalScroll)
	            {
                    Vec2 originalSize = GetComputedLayoutSize();
                    f32 originalHeight = originalSize.height;
                    f32 contentMaxY = contentSize.height;

                    if (contentMaxY > originalHeight + ScrollSizeBuffer)
                    {
                        Rect scrollRect = GetVerticalScrollBarRect();
                        scrollRect = LocalToScreenSpaceRect(scrollRect);

                        if (scrollRect.Contains(globalMousePos))
                        {
                            dragEvent->draggedWidget = this;
                            dragEvent->ConsumeAndStopPropagation(this);
                            isVerticalScrollPressed = true;
                        }
                    }
	            }
            }
            else if (mouseEvent->type == CEventType::DragMove && isVerticalScrollPressed)
            {
                CDragEvent* dragEvent = (CDragEvent*)event;

                if (allowVerticalScroll && isVerticalScrollPressed)
                {
                    Vec2 originalSize = GetComputedLayoutSize();
                    f32 originalHeight = originalSize.height;

                    normalizedScroll.y += mouseDelta.y / (originalHeight - GetVerticalScrollBarRect().GetSize().height);
                    normalizedScroll.y = Math::Clamp01(normalizedScroll.y);

                    dragEvent->ConsumeAndStopPropagation(this);
                    SetNeedsLayout();
                    SetNeedsPaint();
                }
            }
            else if (mouseEvent->type == CEventType::DragEnd && isVerticalScrollPressed)
            {
                CDragEvent* dragEvent = (CDragEvent*)event;
                
                isVerticalScrollPressed = false;
                dragEvent->ConsumeAndStopPropagation(this);
            }
            else if (mouseEvent->type == CEventType::MouseWheel && allowVerticalScroll)
            {
                Vec2 originalSize = GetComputedLayoutSize();
                f32 originalHeight = originalSize.height;
                f32 contentMaxY = contentSize.height;

                if (contentMaxY > originalHeight + ScrollSizeBuffer) // If scrolling is possible
                {
                    normalizedScroll.y += -mouseEvent->wheelDelta.y * scrollSensitivity / (contentMaxY - originalHeight);
                    normalizedScroll.y = Math::Clamp01(normalizedScroll.y);

                    SetNeedsLayout();
                    SetNeedsPaint();
                }
            }

            if (controlRects.NonEmpty() && nativeWindow != nullptr)
            {
                if (mouseEvent->type == CEventType::MousePress && mouseEvent->button == MouseButton::Left)
                {
                    for (int i = 0; i < controlRects.GetSize(); ++i)
                    {
                        if (controlRects[i].Contains(windowSpaceMousePos))
                        {
                            clickedControlIdx = i;
                        }
                    }
                }
                else if (mouseEvent->type == CEventType::MouseRelease && mouseEvent->button == MouseButton::Left)
                {
	                for (int i = 0; i < controlRects.GetSize(); ++i)
	                {
		                if (controlRects[i].Contains(windowSpaceMousePos))
		                {
                            if (i == clickedControlIdx)
                            {
                                if (clickedControlIdx == 0)
                                    nativeWindow->platformWindow->Minimize();
                                else if (clickedControlIdx == 1 && nativeWindow->platformWindow->IsMaximized())
                                    nativeWindow->platformWindow->Restore();
                                else if (clickedControlIdx == 1)
                                    nativeWindow->platformWindow->Maximize();
                                else if (clickedControlIdx == 2)
                                    QueueDestroy();
                            }
		                }
	                }

                    clickedControlIdx = -1;
                }
                else if (mouseEvent->type == CEventType::MouseMove)
                {
                    for (int i = 0; i < controlRects.GetSize(); ++i)
                    {
                        if (controlRects[i].Contains(windowSpaceMousePos))
                        {
                            hoveredControls[i] = true;
                            SetNeedsPaint();
                        }
                        else
                        {
                            if (hoveredControls[i])
                                SetNeedsPaint();
                            hoveredControls[i] = false;
                        }
                    }
                }
                else if (mouseEvent->type == CEventType::MouseLeave)
                {
                    for (int i = 0; i < controlRects.GetSize(); ++i)
                    {
                        if (hoveredControls[i])
                            SetNeedsPaint();
                        hoveredControls[i] = false;
                    }
                }
            }
        }

        Super::HandleEvent(event);
    }

    bool CWindow::IsSubWidgetAllowed(Class* subWidgetClass)
    {
        return subWidgetClass->IsSubclassOf<CWidget>() && !subWidgetClass->IsSubclassOf<CMenuItem>();
    }

    void CWindow::OnSubobjectAttached(Object* object)
    {
        Super::OnSubobjectAttached(object);

        if (!object)
            return;

        if (object->IsOfType<CWidget>())
        {
            ((CWidget*)object)->ownerWindow = this;
        }

        if (object->IsOfType<CMenuItem>())
        {
            menuItems.Add(static_cast<CMenuItem*>(object));
        }
    }

    void CWindow::OnSubobjectDetached(Object* object)
    {
        Super::OnSubobjectDetached(object);
        
        if (!object)
            return;

        if (object->IsOfType<CWidget>())
        {
            ((CWidget*)object)->ownerWindow = nullptr;
        }

    	if (object->IsOfType<CMenuItem>())
        {
            menuItems.Remove(static_cast<CMenuItem*>(object));
        }
    }

} // namespace CE::Widgets
