#include "CrystalWidgets.h"

namespace CE::Widgets
{
    
    CWindow::CWindow()
    {
        receiveMouseEvents = false;
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

    void CWindow::ConstructWindow()
    {
        if (parent != nullptr) // Renderer2D is created only for Root windows
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

        RPI::PerViewConstants viewConstantData{};
        viewConstantData.viewMatrix = Matrix4x4::Identity();
        viewConstantData.projectionMatrix = Matrix4x4::Translation(Vec3(-1, -1, 0)) * 
            Matrix4x4::Scale(Vec3(1.0f / screenWidth, 1.0f / screenHeight, 1)) * 
            Quat::EulerDegrees(0, 0, 0).ToMatrix();

        viewConstantData.viewProjectionMatrix = viewConstantData.projectionMatrix * viewConstantData.viewMatrix;
        viewConstantData.viewPosition = Vec4(0, 0, 0, 0);
        
        renderer->SetScreenSize(Vec2i(screenWidth, screenHeight));
        renderer->SetViewConstants(viewConstantData);
    }

    void CWindow::Tick()
    {
        if (!renderer)
            return;

        // Tick: Input Handling
        Vec2 globalMousePos = InputManager::GetGlobalMousePosition().ToVec2();
        Vec2 mouseDelta = InputManager::GetMouseDelta().ToVec2();
        if (prevMousePos == Vec2())
            prevMousePos = globalMousePos;

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

        std::function<CWidget*(CWidget*)> getBottomMostHoveredWidget = [&](CWidget* widget) -> CWidget*
            {
				if (widget == nullptr)
					return nullptr;

                Rect widgetRect = widget->GetScreenSpaceRect();
                CWindow* ownerWindow = widget->ownerWindow;
				if (ownerWindow)
				{
					if (PlatformWindow* nativeWindow = ownerWindow->GetRootNativeWindow())
                    {
	                    if (!nativeWindow->IsFocussed() || !nativeWindow->IsShown() || nativeWindow->IsMinimized())
	                    {
                            return nullptr;
	                    }
                    }
				}

                if (widgetRect.Contains(globalMousePos))
                {
                    for (int i = widget->attachedWidgets.GetSize() - 1; i >= 0; --i)
                    {
                        if (!widget->attachedWidgets[i]->IsEnabled())
                            continue;
                        CWidget* insideWidget = getBottomMostHoveredWidget(widget->attachedWidgets[i]);
                        if (insideWidget && insideWidget->receiveMouseEvents)
                            return insideWidget;
                    }

                    return widget;
                }

                return nullptr;
            };

        CWidget* hoveredWidget = getBottomMostHoveredWidget(this);

        PlatformWindow* platformWindow = GetRootNativeWindow();
        if (platformWindow && !platformWindow->IsFocussed())
        {
            hoveredWidget = nullptr;
        }

        if (hoveredWidgetsStack.NonEmpty() && hoveredWidgetsStack.Top() != hoveredWidget &&
            (hoveredWidget == nullptr || !hoveredWidgetsStack.Top()->SubWidgetExistsRecursive(hoveredWidget)))
        {
            CMouseEvent mouseEvent{};
            mouseEvent.name = "MouseLeave";
            mouseEvent.type = CEventType::MouseLeave;
            mouseEvent.button = curButton;
            mouseEvent.mousePos = globalMousePos;
            mouseEvent.prevMousePos = prevMousePos;
            mouseEvent.direction = CEventDirection::BottomToTop;

            while (hoveredWidgetsStack.NonEmpty() && hoveredWidgetsStack.Top() != hoveredWidget)
            {
                mouseEvent.sender = hoveredWidgetsStack.Top();
                mouseEvent.Reset();
                if (hoveredWidgetsStack.Top()->receiveMouseEvents)
                {
	                hoveredWidgetsStack.Top()->HandleEvent(&mouseEvent);
                }
                hoveredWidgetsStack.Pop();
            }
        }
        
        if (hoveredWidget != nullptr && (hoveredWidgetsStack.IsEmpty() || hoveredWidgetsStack.Top() != hoveredWidget) &&
            (hoveredWidgetsStack.IsEmpty() || !hoveredWidget->SubWidgetExistsRecursive(hoveredWidgetsStack.Top())))
        {
            CMouseEvent mouseEvent{};
            mouseEvent.name = "MouseEnter";
            mouseEvent.type = CEventType::MouseEnter;
            mouseEvent.button = curButton;
            mouseEvent.mousePos = globalMousePos;
            mouseEvent.prevMousePos = prevMousePos;
            mouseEvent.direction = CEventDirection::BottomToTop;

            int idx = hoveredWidgetsStack.GetSize();
            CWidget* basePrevWidget = nullptr;
            if (hoveredWidgetsStack.NonEmpty())
                basePrevWidget = hoveredWidgetsStack.Top();

            auto widget = hoveredWidget;
            
            while ((hoveredWidgetsStack.IsEmpty() || widget != basePrevWidget) && widget != nullptr)
            {
                hoveredWidgetsStack.InsertAt(idx, widget);
                widget = widget->parent;
            }

            for (int i = idx; i < hoveredWidgetsStack.GetSize(); i++)
            {
                mouseEvent.Reset();
                mouseEvent.sender = hoveredWidgetsStack[i];
                if (hoveredWidgetsStack[i]->receiveMouseEvents)
                {
                    hoveredWidgetsStack[i]->HandleEvent(&mouseEvent);
                }
            }
        }


        if (abs(mouseDelta.x) >= FLT_EPSILON || abs(mouseDelta.y) >= FLT_EPSILON)
        {
            CMouseEvent mouseEvent{};
            mouseEvent.name = "MouseMove";
            mouseEvent.type = CEventType::MouseMove;
            mouseEvent.button = curButton;
            mouseEvent.mousePos = globalMousePos;
            mouseEvent.prevMousePos = prevMousePos;
            mouseEvent.direction = CEventDirection::BottomToTop;

            if (hoveredWidgetsStack.NonEmpty())
            {
	            mouseEvent.sender = hoveredWidgetsStack.Top();
                hoveredWidgetsStack.Top()->HandleEvent(&mouseEvent);
            }

            if (draggedWidget != nullptr)
            {
                CDragEvent dragEvent{};
                dragEvent.name = "DragEvent";
                dragEvent.type = CEventType::DragMove;
                dragEvent.button = MouseButton::Left;
                dragEvent.mousePos = globalMousePos;
                dragEvent.prevMousePos = prevMousePos;
                dragEvent.direction = CEventDirection::BottomToTop;
                dragEvent.isInside = true;

                dragEvent.sender = nullptr;
                if (hoveredWidgetsStack.NonEmpty())
                    dragEvent.sender = hoveredWidgetsStack.Top();
                dragEvent.draggedWidget = draggedWidget;

                draggedWidget->HandleEvent(&dragEvent);

                // Cancel dragging if any of the CDragEvent's are not consumed by the dragged widget
                if (!dragEvent.isConsumed)
                {
                    draggedWidget = nullptr;
                }
                else
                {
                    draggedWidget = dragEvent.draggedWidget;
                }
            }
        }

        Enum* mouseButtonEnum = GetStaticEnum<MouseButton>();
        for (int i = 0; i < mouseButtonEnum->GetConstantsCount(); ++i)
        {
            MouseButton mouseButton = (MouseButton)mouseButtonEnum->GetConstant(i)->GetValue();
	        if (InputManager::IsMouseButtonDown(mouseButton))
	        {
                CMouseEvent event{};
                event.name = "MousePress";
                event.type = CEventType::MousePress;
                event.button = mouseButton;
                event.mousePos = globalMousePos;
                event.prevMousePos = prevMousePos;
                event.direction = CEventDirection::BottomToTop;
                event.isInside = true;

                if (hoveredWidgetsStack.NonEmpty())
                {
                    event.sender = hoveredWidgetsStack.Top();
                    widgetsPressedPerMouseButton[i] = event.sender;
                    hoveredWidgetsStack.Top()->HandleEvent(&event);

                    if (hoveredWidgetsStack.Top()->receiveDragEvents && mouseButton == MouseButton::Left)
                    {
                        CDragEvent dragEvent{};
                        dragEvent.name = "DragEvent";
                        dragEvent.type = CEventType::DragBegin;
                        dragEvent.button = mouseButton;
                        dragEvent.mousePos = globalMousePos;
                        dragEvent.prevMousePos = prevMousePos;
                        dragEvent.direction = CEventDirection::BottomToTop;
                        dragEvent.isInside = true;

                        dragEvent.sender = hoveredWidgetsStack.Top();
                        dragEvent.draggedWidget = hoveredWidgetsStack.Top();
                        hoveredWidgetsStack.Top()->HandleEvent(&dragEvent);

						if (dragEvent.isConsumed)
						{
							draggedWidget = dragEvent.draggedWidget;
						}
                    }
                }
	        }

            if (InputManager::IsMouseButtonUp(mouseButton))
            {
                CMouseEvent event{};
                event.name = "MouseRelease";
                event.type = CEventType::MouseRelease;
                event.button = mouseButton;
                event.mousePos = globalMousePos;
                event.prevMousePos = prevMousePos;
                event.direction = CEventDirection::BottomToTop;
                event.isInside = true;

                if (hoveredWidgetsStack.NonEmpty())
                {
                    event.sender = hoveredWidgetsStack.Top();
                    hoveredWidgetsStack.Top()->HandleEvent(&event);
                }

                if (widgetsPressedPerMouseButton[i] != nullptr)
                {
                    event.Reset();
                    event.isInside = false;
                    widgetsPressedPerMouseButton[i]->HandleEvent(&event);
                }

                if (draggedWidget != nullptr)
                {
                    CDragEvent dragEvent{};
                    dragEvent.name = "DragEvent";
                    dragEvent.type = CEventType::DragEnd;
                    dragEvent.button = mouseButton;
                    dragEvent.mousePos = globalMousePos;
                    dragEvent.prevMousePos = prevMousePos;
                    dragEvent.direction = CEventDirection::BottomToTop;
                    dragEvent.isInside = true;

                    dragEvent.sender = nullptr;
                    if (hoveredWidgetsStack.NonEmpty())
                        dragEvent.sender = hoveredWidgetsStack.Top();
                    dragEvent.draggedWidget = draggedWidget;
                    dragEvent.dropTarget = dragEvent.sender;
                    draggedWidget->HandleEvent(&dragEvent);

                    draggedWidget = nullptr;
                }

                widgetsPressedPerMouseButton[i] = nullptr;
            }
        }

        prevMousePos = globalMousePos;

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

    void CWindow::OnPaint(CPaintEvent* paintEvent)
    {
        Super::OnPaint(paintEvent);
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
