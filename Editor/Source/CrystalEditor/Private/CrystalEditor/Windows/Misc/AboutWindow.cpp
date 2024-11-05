#include "CrystalEditor.h"

namespace CE::Editor
{
    static AboutWindow* instance = nullptr;

    AboutWindow::AboutWindow()
    {

    }

    void AboutWindow::Construct()
    {
        Super::Construct();

        titleBar->Height(30);
        titleBarLabel->FontSize(13);

        FBrush splash = FBrush("/Editor/Assets/Images/Splash");
        splash.SetBrushTiling(FBrushTiling::TileXY);
        splash.SetHAlign(HAlign::Center);
        splash.SetVAlign(VAlign::Center);

        (*this)
	        .Title("About Crystal editor")
	        .MinimizeEnabled(false)
			.MaximizeEnabled(false)
			.ContentPadding(Vec4(1, 1, 1, 1) * 20)
	        .Content(
                FNew(FImage)
                .Background(splash)
                .HAlign(HAlign::Fill)
                .Height(300)
                .Margin(Vec4(0, 0, 0, 10)),

                FNew(FLabel)
                .FontSize(15)
                .Text("CrystalEditor")
                .HAlign(HAlign::Left),

                FNew(FLabel)
                .FontSize(13)
                .Text("Version: " CE_ENGINE_VERSION_STRING_SHORT " pre-alpha [Build " CE_TOSTRING(CE_VERSION_BUILD) "]")
                .HAlign(HAlign::Left),

                FNew(FLabel)
                .FontSize(13)
                .Text("Platform: " + PlatformMisc::GetOSVersionString())
                .HAlign(HAlign::Left),

                FNew(FLabel)
                .FontSize(13)
                .Text("Icons by flaticon.com")
                .HAlign(HAlign::Left)
	        )
        ;
    }

    void AboutWindow::OnBeforeDestroy()
    {
	    Super::OnBeforeDestroy();

        if (this == instance)
        {
            instance = nullptr;
        }
    }

    AboutWindow* AboutWindow::Show()
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

        PlatformWindow* window = PlatformApplication::Get()->CreatePlatformWindow("About", 600, 500, info);
        window->SetAlwaysOnTop(true);
        window->SetBorderless(true);

        FRootContext* rootContext = FusionApplication::Get()->GetRootContext();
        FFusionContext* parentContext = rootContext;
        if (rootContext->GetChildContexts().NonEmpty())
        {
            // FRootContext should have only 1 NativeContext which is the primary Native Window
            parentContext = rootContext->GetChildContexts().GetFirst();
        }

        FNativeContext* context = FNativeContext::Create(window, "AboutWindow", parentContext);
        parentContext->AddChildContext(context);

        AboutWindow* aboutWindow = nullptr;

        FAssignNewOwned(AboutWindow, aboutWindow, context);
        context->SetOwningWidget(aboutWindow);

        return aboutWindow;
    }
}

