#include "CrystalEditor.h"

namespace CE::Editor
{
    static SampleWidgetWindow* instance = nullptr;

    SampleWidgetWindow::SampleWidgetWindow()
    {

    }

    void SampleWidgetWindow::Construct()
    {
        Super::Construct();

        titleBar->Height(30);
        titleBarLabel->FontSize(13);

        constexpr auto splashImage = "/Editor/Assets/Images/Splash";

        instance = this;

        FBrush splash = FBrush(splashImage);
        splash.SetBrushTiling(FBrushTiling::TileXY);
        splash.SetImageFit(FImageFit::Contain);
        splash.SetBrushPosition(Vec2(0.5f, 0.5f)); // 50%, 50%
        splash.SetBrushSize(Vec2(512, 288));

        (*this)
            .Title("Fusion Samples")
            .MinimizeEnabled(false)
            .MaximizeEnabled(false)
            .ContentPadding(Vec4(1, 1, 1, 1) * 20)
            .Content(
                FNew(FImage)
                .Background(splash)
                .BackgroundShape(FRoundedRectangle(5, 10, 15, 20))
                .HAlign(HAlign::Fill)
                .Height(350)
                .Margin(Vec4(0, 0, 0, 10)),

                FNew(FLabel)
                .FontSize(13)
                .Text("Version: " CE_ENGINE_VERSION_STRING_SHORT " pre-alpha [Build " CE_TOSTRING(CE_VERSION_BUILD) "]")
                .HAlign(HAlign::Left),

                FNew(FLabel)
                .FontSize(13)
                .Text("Platform: " + PlatformMisc::GetOSVersionString())
                .HAlign(HAlign::Left),

                FNew(FSplitBox)
                .ContentVAlign(VAlign::Center)
                .HAlign(HAlign::Fill)
                .Name("DebugSplitBox")
                (
                    FNew(FStyledWidget)
                    .Background(Color::Yellow())
                    .Height(20)
                    .FillRatio(0.25f),

                    FNew(FStyledWidget)
                    .Background(Color::Green())
                    .Height(20)
                    .FillRatio(0.25f),

                    FNew(FStyledWidget)
                    .Background(Color::Cyan())
                    .Height(20)
                    .FillRatio(0.5f)
                )
            )
            ;
    }

    void SampleWidgetWindow::OnBeforeDestroy()
    {
	    Super::OnBeforeDestroy();

        if (this == instance)
        {
            instance = nullptr;
        }
    }

    SampleWidgetWindow* SampleWidgetWindow::Show()
    {
        if (instance != nullptr)
        {
            FNativeContext* nativeContext = static_cast<FNativeContext*>(instance->GetContext());
            PlatformWindow* window = nativeContext->GetPlatformWindow();
            window->SetAlwaysOnTop(true);
            window->Show();
            return instance;
        }

        PlatformWindowInfo info{
            .maximised = false,
            .fullscreen = false,
            .resizable = false,
            .hidden = false,
            .windowFlags = PlatformWindowFlags::Utility | PlatformWindowFlags::DestroyOnClose | PlatformWindowFlags::SkipTaskbar
        };

        PlatformWindow* window = PlatformApplication::Get()->CreatePlatformWindow("SampleWidgets", 600, 600, info);
        window->SetAlwaysOnTop(true);
        window->SetBorderless(true);

        FRootContext* rootContext = FusionApplication::Get()->GetRootContext();
        FFusionContext* parentContext = rootContext;
        if (rootContext->GetChildContexts().NonEmpty())
        {
            // FRootContext should have only 1 NativeContext which is the primary Native Window
            parentContext = rootContext->GetChildContexts().GetFirst();
        }

        FNativeContext* context = FNativeContext::Create(window, "SampleWidgetWindow", parentContext);
        parentContext->AddChildContext(context);

        SampleWidgetWindow* sampleWindow = nullptr;

        FAssignNewOwned(SampleWidgetWindow, sampleWindow, context);
        context->SetOwningWidget(sampleWindow);

        return sampleWindow;
    }
}

