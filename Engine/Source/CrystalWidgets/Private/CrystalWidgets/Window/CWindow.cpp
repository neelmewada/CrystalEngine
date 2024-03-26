#include "CrystalWidgets.h"

namespace CE::Widgets
{
    
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
        CE_LOG(Info, All, "Resize: {}x{}", screenWidth, screenHeight);

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
