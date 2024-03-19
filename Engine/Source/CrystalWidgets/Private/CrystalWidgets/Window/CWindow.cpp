#include "CrystalWidgets.h"

namespace CE::Widgets
{
    
    CWindow::CWindow()
    {
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

        if (windowResizeDelegate != 0)
        {
            PlatformApplication::Get()->onWindowDrawableSizeChanged.RemoveDelegateInstance(windowResizeDelegate);
        }
    }

    void CWindow::SetPlatformWindow(PlatformWindow* window)
    {
        this->nativeWindow = window;
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

    void CWindow::ConstructWindow()
    {
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
