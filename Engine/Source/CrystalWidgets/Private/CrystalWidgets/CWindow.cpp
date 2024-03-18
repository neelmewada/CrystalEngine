#include "CrystalWidgets.h"

namespace CE::Widgets
{
    
    CWindow::CWindow()
    {
        if (!IsDefaultInstance())
        {
            title = GetName().GetString();

            painter = CreateDefaultSubobject<CPainter>("Painter");

            windowResizeDelegate =
                PlatformApplication::Get()->onWindowDrawableSizeChanged.AddDelegateInstance(
                    MemberDelegate(&Self::OnWindowSizeChanged, this));
        }
    }

    CWindow::~CWindow()
    {
        RPISystem::Get().GetDrawListTagRegistry()->ReleaseTag(drawListTag);
        drawListTag = 0;

        if (windowResizeDelegate != 0)
        {
            PlatformApplication::Get()->onWindowDrawableSizeChanged.RemoveDelegateInstance(windowResizeDelegate);
        }
    }

    void CWindow::SetPlatformWindow(PlatformWindow* window)
    {
        this->nativeWindow = window;

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
            PlatformWindow* platformWindow = nativeWindow;
            if (platformWindow == nullptr)
                platformWindow = platformApp->GetMainWindow();
            
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

    const Array<RHI::DrawPacket*>& CWindow::FlushDrawPackets(u32 imageIndex)
    {
        static const Array<RHI::DrawPacket*> empty{};
        if (!IsVisible() || !IsEnabled())
        {
            return empty;
        }

        return renderer->FlushDrawPackets(imageIndex);
    }

    void CWindow::SetWindowType(CWindowType windowType)
    {
        this->windowType = windowType;

        SetNeedsPaintRecursively(true);
    }

    void CWindow::OnWindowSizeChanged(PlatformWindow* window, u32 newWidth, u32 newHeight)
    {
        if (!nativeWindow || !renderer)
            return;

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

        // Painting
        if (NeedsPaint())
        {
            SetNeedsPaintRecursively(false);
            
            painter->renderer = renderer;

            renderer->Begin();
            {
                renderer->PushFont(CApplication::Get()->defaultFontName, 16);

                // renderer->SetFillColor(Color::FromRGBA32(21, 21, 21));
                //
                // u32 w = 0, h = 0;
                // nativeWindow->GetDrawableWindowSize(&w, &h);
                //
                // if (nativeWindow->IsBorderless())
                // {
                //     renderer->SetBorderThickness(2.0f);
                //     renderer->SetOutlineColor(Color::FromRGBA32(48, 48, 48));
                // }
                //
                // renderer->SetCursor(Vec2(0, 0));
                // renderer->DrawRect(Vec2(w, h));
                //
                // // Draw Tab
                //
                // renderer->SetBorderThickness(0.0f);
                // renderer->SetFillColor(Color::FromRGBA32(36, 36, 36));
                // renderer->SetCursor(Vec2(20, 2.5f));
                // renderer->DrawRoundedRect(Vec2(100, 35), Vec4(5, 5, 0, 0));
                //
                // renderer->SetOutlineColor(Color::White());
                // renderer->DrawText(title, Vec2(100, 35));

                auto thisPaint = CPaintEvent();
                thisPaint.painter = painter;
                thisPaint.sender = this;
                thisPaint.name = "PaintEvent";
                
                this->HandleEvent(&thisPaint);
                
                renderer->PopFont();
            }
            renderer->End();
        }

        // TODO: Input Handling

    }

    void CWindow::OnBeforeDestroy()
    {
        Super::OnBeforeDestroy();

        delete renderer; renderer = nullptr;
    }

    void CWindow::Construct()
    {
        Super::Construct();

        
    }

    void CWindow::OnPaint(CPaintEvent* paintEvent)
    {
        Super::OnPaint(paintEvent);

        CPainter* painter = paintEvent->painter;

        u32 w = 0, h = 0;
        nativeWindow->GetDrawableWindowSize(&w, &h);

        // - Draw Background -

        CPen pen = CPen(Color::FromRGBA32(48, 48, 48));
        CBrush brush = CBrush(Color::FromRGBA32(21, 21, 21));
        CFont font = CFont("Roboto", 12, false);

        painter->SetBrush(brush);

        if (nativeWindow->IsBorderless())
        {
            pen.SetWidth(2.0f);
            painter->SetPen(pen);
        }

        painter->DrawRect(Rect::FromSize(0, 0, w, h));

        // - Draw Tab -

        pen.SetWidth(0.0f);
        pen.SetColor(Color::Clear());

        brush.SetColor(Color::FromRGBA32(36, 36, 36));

        painter->SetPen(pen);
        painter->SetBrush(brush);
        painter->SetFont(font);

        Rect tabRect = Rect::FromSize(20, 2.5f, 100, 35);
        painter->DrawRoundedRect(tabRect, Vec4(5, 5, 0, 0));

        pen.SetColor(Color::White());
        painter->SetPen(pen);

        painter->DrawText(title, tabRect);

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
