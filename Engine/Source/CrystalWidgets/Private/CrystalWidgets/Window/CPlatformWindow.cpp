#include "CrystalWidgets.h"

namespace CE::Widgets
{

    CPlatformWindow::CPlatformWindow(CWindow* owner, u32 width, u32 height, const PlatformWindowInfo& info, CPlatformWindow* parentWindow)
	    : owner(owner), parentWindow(parentWindow)
    {
        platformWindow = PlatformApplication::Get()->CreatePlatformWindow("", width, height, info);

        Init();
    }

    CPlatformWindow::CPlatformWindow(CWindow* owner, PlatformWindow* nativeWindow, CPlatformWindow* parentWindow)
	    : owner(owner), platformWindow(nativeWindow), parentWindow(parentWindow)
    {
        Init();
    }

    CPlatformWindow::~CPlatformWindow()
    {
        if (parentWindow)
        {
	        parentWindow->childrenWindows.Remove(this);
        }

        isDeleted = true;
        painter->Destroy();
        painter = nullptr;

        if (renderer || swapChain)
        {
            CApplication::Get()->GetFrameScheduler()->WaitUntilIdle();
        }

        delete renderer; renderer = nullptr;

        if (owner)
        {
	        owner->nativeWindow = nullptr;
        }

        PlatformApplication::Get()->onWindowDrawableSizeChanged.RemoveDelegateInstance(windowResizeDelegate);
        windowResizeDelegate = 0;

        RPISystem::Get().GetDrawListTagRegistry()->ReleaseTag(drawListTag);

        CApplication::Get()->platformWindows.Remove(this);
        if (swapChain)
        {
            delete swapChain; swapChain = nullptr;
        }

	    if (platformWindow)
	    {
            PlatformApplication::Get()->DestroyWindow(platformWindow);
            platformWindow = nullptr;
	    }
    }

    void CPlatformWindow::Init()
    {
        if (parentWindow)
        {
            parentWindow->childrenWindows.Add(this);
        }

	    {
		    RHI::SwapChainDescriptor desc{};
	    	desc.imageCount = CApplication::Get()->numFramesInFlight;
	    	desc.preferredFormats = { RHI::Format::R8G8B8A8_UNORM, RHI::Format::B8G8R8A8_UNORM };

	    	platformWindow->GetWindowSize(&desc.preferredWidth, &desc.preferredHeight);

	    	swapChain = RHI::gDynamicRHI->CreateSwapChain(platformWindow, desc);
	    }

        CApplication::Get()->platformWindows.Add(this);

	    {
            auto app = CApplication::Get();
            auto platformApp = PlatformApplication::Get();

            Renderer2DDescriptor desc{};
            desc.drawShader = app->draw2dShader;
            desc.multisampling.sampleCount = 1;
            desc.multisampling.quality = 1.0f;
            desc.drawListTag = drawListTag = RPISystem::Get().GetDrawListTagRegistry()->AcquireTag(owner->GetName());
            desc.drawShader = CApplication::Get()->draw2dShader;
            desc.numFramesInFlight = CApplication::Get()->numFramesInFlight;

            desc.initialDrawItemStorage = 1'000;
            desc.drawItemStorageIncrement = 1'000;

            u32 screenWidth = 0, screenHeight = 0;
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

            painter = CreateObject<CPainter>(nullptr, "Painter", OF_Transient);

            windowResizeDelegate =
                PlatformApplication::Get()->onWindowDrawableSizeChanged.AddDelegateInstance(
                    MemberDelegate(&CPlatformWindow::OnWindowSizeChanged, this));
	    }
    }

    void CPlatformWindow::OnWindowSizeChanged(PlatformWindow* window, u32 newWidth, u32 newHeight)
    {
        if (!platformWindow || !renderer)
            return;

        if (swapChain)
        {
            swapChain->Rebuild();
        }

        u32 screenWidth = 0; u32 screenHeight = 0;
        platformWindow->GetDrawableWindowSize(&screenWidth, &screenHeight);
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

        owner->SetNeedsLayout();
        owner->SetNeedsPaint();
    }

    void CPlatformWindow::Tick()
    {
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

        if (platformWindow != nullptr)
        {
            if (IsFocused() && !owner->IsFocussed())
            {
                CFocusEvent focusEvent{};
                focusEvent.name = "GotFocus";
                focusEvent.gotFocus = true;
                focusEvent.type = CEventType::FocusChanged;
                focusEvent.focusedWidget = owner;

                owner->HandleEvent(&focusEvent);
            }
            else if (!IsFocused() && owner->IsFocussed())
            {
                CFocusEvent focusEvent{};
                focusEvent.name = "LostFocus";
                focusEvent.gotFocus = false;
                focusEvent.type = CEventType::FocusChanged;
                focusEvent.focusedWidget = owner;
                focusEvent.direction = CEventDirection::TopToBottom;

                owner->HandleEvent(&focusEvent);
            }
        }

        auto prevTime = clock();

        // Tick: Styling
        owner->UpdateStyleIfNeeded();

        auto styleTime = (f32)(clock() - prevTime) / CLOCKS_PER_SEC;
        prevTime = clock();

        // Tick: Layout
        owner->UpdateLayoutIfNeeded();

        auto layoutTime = (f32)(clock() - prevTime) / CLOCKS_PER_SEC;
        prevTime = clock();

        // Tick: Painting
        if (owner->NeedsPaint())
        {
            owner->SetNeedsPaintRecursively(false);

            painter->renderer = renderer;

            renderer->Begin();
            {
                renderer->PushFont(CApplication::Get()->defaultFontName, 16);

                auto thisPaint = CPaintEvent();
                thisPaint.painter = painter;
                thisPaint.sender = owner;
                thisPaint.name = "PaintEvent";

                owner->HandleEvent(&thisPaint);

                renderer->PopFont();
            }
            renderer->End();
        }

        auto paintTime = (f32)(clock() - prevTime) / CLOCKS_PER_SEC;

        CE_LOG(Info, All, "Total Time: {} | {} | {}", styleTime, layoutTime, paintTime);
    }

    void CPlatformWindow::GetWindowSize(u32* preferredWidth, u32* preferredHeight)
    {
        platformWindow->GetDrawableWindowSize(preferredWidth, preferredHeight);
    }

    void CPlatformWindow::Show()
    {
        platformWindow->Show();

        CApplication::Get()->SetFocus(owner);
    }

    void CPlatformWindow::Hide()
    {
        platformWindow->Hide();
    }

    bool CPlatformWindow::IsShown()
    {
        return platformWindow->IsShown();
    }

    bool CPlatformWindow::IsHidden()
    {
        return platformWindow->IsHidden();
    }

    bool CPlatformWindow::IsFocused()
    {
        if (platformWindow->IsFocused())
            return true;

        for (CPlatformWindow* childWindow : childrenWindows)
        {
            if (childWindow && childWindow->IsFocused())
                return true;
        }

        return false;
    }

    bool CPlatformWindow::IsMinimized()
    {
        return platformWindow->IsMinimized();
    }

    bool CPlatformWindow::IsAlwaysOnTop()
    {
        return platformWindow->IsAlwaysOnTop();
    }

    const Array<RHI::DrawPacket*>& CPlatformWindow::FlushDrawPackets(u32 imageIndex)
    {
        static const Array<RHI::DrawPacket*> empty{};
        if (!owner->IsVisible() || !owner->IsEnabled())
        {
            return empty;
        }

        return renderer->FlushDrawPackets(imageIndex);
    }

} // namespace CE::Widgets
