#include "CrystalWidgets.h"

namespace CE::Widgets
{
    constexpr f32 ScrollRectWidth = 10.0f;
    constexpr f32 MinScrollRectSize = 20.0f;
    
    CWindow::CWindow()
    {
        receiveMouseEvents = false;
        clipChildren = true;

        painter = CreateDefaultSubobject<CPainter>("Painter");

        if (!IsDefaultInstance())
        {
            title = GetName().GetString();

            windowResizeDelegate =
                PlatformApplication::Get()->onWindowDrawableSizeChanged.AddDelegateInstance(
                    MemberDelegate(&Self::OnWindowSizeChanged, this));
        }
    }

    CWindow::~CWindow()
    {
        RPISystem::Get().GetDrawListTagRegistry()->ReleaseTag(drawListTag);
        drawListTag = 0;

        delete swapChain; swapChain = nullptr;

        if (windowResizeDelegate != 0)
        {
            PlatformApplication::Get()->onWindowDrawableSizeChanged.RemoveDelegateInstance(windowResizeDelegate);
        }
    }

    void CWindow::SetPlatformWindow(PlatformWindow* window)
    {
        this->nativeWindow = window;

        if (!swapChain && nativeWindow)
        {
            receiveMouseEvents = true;

            RHI::SwapChainDescriptor desc{};
            desc.imageCount = CApplication::Get()->numFramesInFlight;
            desc.preferredFormats = { RHI::Format::R8G8B8A8_UNORM, RHI::Format::B8G8R8A8_UNORM };
            
            nativeWindow->GetWindowSize(&desc.preferredWidth, &desc.preferredHeight);

            swapChain = RHI::gDynamicRHI->CreateSwapChain(nativeWindow, desc);
        }

        OnPlatformWindowSet();
    }

    const Array<RHI::DrawPacket*>& CWindow::FlushDrawPackets(u32 imageIndex)
    {
        static const Array<RHI::DrawPacket*> empty{};
        if (!IsVisible() || !IsEnabled())
        {
            return empty;
        }
        
        return renderer->FlushDrawPackets(imageIndex);
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

    void CWindow::ConstructWindow()
    {
        if (parent != nullptr) // Renderer2D is created only for root native windows
            return;
        if (nativeWindow == nullptr)
            return;

        if (renderer == nullptr)
        {
            auto app = CApplication::Get();
            auto platformApp = PlatformApplication::Get();

            Renderer2DDescriptor desc{};
            desc.drawShader = app->draw2dShader;
            desc.multisampling.sampleCount = 1;
            desc.multisampling.quality = 1.0f;
            desc.drawListTag = drawListTag = RPISystem::Get().GetDrawListTagRegistry()->AcquireTag(GetName());
            desc.drawShader = CApplication::Get()->draw2dShader;
            desc.numFramesInFlight = CApplication::Get()->numFramesInFlight;
            
            desc.initialDrawItemStorage = 1'000;
            desc.drawItemStorageIncrement = 1'000;

            u32 screenWidth = 0, screenHeight = 0;
            PlatformWindow* platformWindow = GetRootNativeWindow();
            CE_ASSERT(platformWindow != nullptr, "CWindow could not find a PlatformWindow in parent hierarchy!");

            platformWindow->GetDrawableWindowSize(&screenWidth, &screenHeight);

            desc.viewConstantData.viewMatrix = Matrix4x4::Identity();
            desc.viewConstantData.projectionMatrix = Matrix4x4::Translation(Vec3(-1, -1, 0)) *
                Matrix4x4::Scale(Vec3(1.0f / screenWidth, 1.0f / screenHeight, 1)) *
                Quat::EulerDegrees(0, 0, 0).ToMatrix();
            desc.viewConstantData.viewProjectionMatrix = desc.viewConstantData.projectionMatrix * desc.viewConstantData.viewMatrix;
            desc.viewConstantData.viewPosition = Vec4(0, 0, 0, 0);
            desc.viewConstantData.pixelResolution = Vec2(screenWidth, screenHeight);
            desc.screenSize = Vec2i(screenWidth, screenHeight);

            renderer = new RPI::Renderer2D(desc);
            
            for (auto [family, fontAtlas] : app->registeredFonts)
            {
                renderer->RegisterFont(family, fontAtlas);
            }
        }
        else
        {
            u32 screenWidth = 0, screenHeight = 0;
            nativeWindow->GetDrawableWindowSize(&screenWidth, &screenHeight);

            OnWindowSizeChanged(nativeWindow, screenWidth, screenHeight);
        }
    }

    void CWindow::OnWindowSizeChanged(PlatformWindow* window, u32 newWidth, u32 newHeight)
    {
        if (!nativeWindow || !renderer)
            return;

        if (swapChain)
        {
            swapChain->Rebuild();
        }

        u32 screenWidth = 0; u32 screenHeight = 0;
        nativeWindow->GetDrawableWindowSize(&screenWidth, &screenHeight);
        //CE_LOG(Info, All, "Resize: {}x{}", screenWidth, screenHeight);

        RPI::PerViewConstants viewConstantData{};
        viewConstantData.viewMatrix = Matrix4x4::Identity();
        viewConstantData.projectionMatrix = Matrix4x4::Translation(Vec3(-1, -1, 0)) * 
            Matrix4x4::Scale(Vec3(1.0f / screenWidth, 1.0f / screenHeight, 1)) * 
            Quat::EulerDegrees(0, 0, 0).ToMatrix();

        viewConstantData.viewProjectionMatrix = viewConstantData.projectionMatrix * viewConstantData.viewMatrix;
        viewConstantData.viewPosition = Vec4(0, 0, 0, 0);
        viewConstantData.pixelResolution = Vec2(screenWidth, screenHeight);
        
        renderer->SetScreenSize(Vec2i(screenWidth, screenHeight));
        renderer->SetViewConstants(viewConstantData);

        SetNeedsLayout();
        SetNeedsPaint();
    }

    void CWindow::Tick()
    {
        if (!renderer)
            return;

        // Tick: Input Handling
        Vec2 globalMousePos = InputManager::GetGlobalMousePosition().ToVec2();
        Vec2 mouseDelta = InputManager::GetMouseDelta().ToVec2();
        //if (prevMousePos == Vec2())
    	//  prevMousePos = globalMousePos;

        MouseButton curButton = MouseButton::None;
        if (InputManager::IsMouseButtonHeld(MouseButton::Left))
        {
            curButton = MouseButton::Left;
        }
        else if (InputManager::IsMouseButtonHeld(MouseButton::Right))
        {
            curButton = MouseButton::Right;
        }
        else if (InputManager::IsMouseButtonHeld(MouseButton::Middle))
        {
            curButton = MouseButton::Middle;
        }

        if (nativeWindow != nullptr)
        {
	        if (nativeWindow->IsFocussed() && !IsFocussed())
	        {
                CFocusEvent focusEvent{};
                focusEvent.name = "GotFocus";
                focusEvent.gotFocus = true;
                focusEvent.type = CEventType::FocusChanged;
                focusEvent.focusedWidget = this;

                HandleEvent(&focusEvent);
	        }
            else if (!nativeWindow->IsFocussed() && IsFocussed())
            {
                CFocusEvent focusEvent{};
                focusEvent.name = "LostFocus";
                focusEvent.gotFocus = false;
                focusEvent.type = CEventType::FocusChanged;
                focusEvent.focusedWidget = this;
                focusEvent.direction = CEventDirection::TopToBottom;

                HandleEvent(&focusEvent);
            }
        }

        // Tick: Styling
        UpdateStyleIfNeeded();

        // Tick: Layout
        UpdateLayoutIfNeeded();

        // Tick: Painting
        if (NeedsPaint())
        {
            SetNeedsPaintRecursively(false);
            
            painter->renderer = renderer;

            renderer->Begin();
            {
                renderer->PushFont(CApplication::Get()->defaultFontName, 16);

                auto thisPaint = CPaintEvent();
                thisPaint.painter = painter;
                thisPaint.sender = this;
                thisPaint.name = "PaintEvent";
                
                this->HandleEvent(&thisPaint);
                
                renderer->PopFont();
            }
            renderer->End();
        }
    }

    void CWindow::OnBeforeDestroy()
    {
        Super::OnBeforeDestroy();

        if (!IsDefaultInstance())
        {
            CApplication::Get()->GetFrameScheduler()->WaitUntilIdle();
        }

        delete renderer; renderer = nullptr;
    }

    void CWindow::Construct()
    {
        Super::Construct();

        
    }

    Vec2 CWindow::CalculateIntrinsicSize(f32 width, f32 height)
    {
        return Vec2(windowSize.x <= 0 ? YGUndefined : windowSize.x, (allowVerticalScroll || windowSize.y <= 0) ? YGUndefined : windowSize.y);
    }

    bool CWindow::IsDockSpace()
    {
        return IsOfType<CDockSpace>();
    }

    PlatformWindow* CWindow::GetRootNativeWindow()
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

	            if (!nativeWindow->IsMaximized())
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

        if (allowVerticalScroll)
        {
            Vec2 originalSize = GetComputedLayoutSize();
            f32 originalHeight = originalSize.height;
            f32 contentMaxY = contentSize.height;

            if (contentMaxY > originalHeight)
            {
                Rect scrollRect = GetVerticalScrollBarRect();

                CPen pen{};
                CBrush brush = CBrush(Color::RGBA(87, 87, 87));

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

            if (contentMaxY > originalHeight)
            {
                Rect scrollRegion = Rect::FromSize(Vec2(originalSize.width - ScrollRectWidth, 0), Vec2(ScrollRectWidth, originalHeight));
                f32 scrollRectHeightRatio = originalHeight / contentMaxY;
                //f32 normalizedScrollY = scrollOffset.y / (contentMaxY - originalHeight);

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

            if (mouseEvent->type == CEventType::DragBegin && (allowVerticalScroll || allowHorizontalScroll))
            {
                CDragEvent* dragEvent = (CDragEvent*)event;

	            if (allowVerticalScroll)
	            {
                    Vec2 originalSize = GetComputedLayoutSize();
                    f32 originalHeight = originalSize.height;
                    f32 contentMaxY = contentSize.height;

                    if (contentMaxY > originalHeight)
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

                if (contentMaxY > originalHeight) // If scrolling is possible
                {
                    normalizedScroll.y += -mouseEvent->wheelDelta.y * scrollSensitivity * 0.1f;
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
                                    nativeWindow->Minimize();
                                else if (clickedControlIdx == 1 && nativeWindow->IsMaximized())
                                    nativeWindow->Restore();
                                else if (clickedControlIdx == 1)
                                    nativeWindow->Maximize();
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

    void CWindow::OnSubobjectAttached(Object* object)
    {
        Super::OnSubobjectAttached(object);

        if (!object)
            return;

        if (object->IsOfType<CWidget>())
        {
            ((CWidget*)object)->ownerWindow = this;
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
    }

} // namespace CE::Widgets
