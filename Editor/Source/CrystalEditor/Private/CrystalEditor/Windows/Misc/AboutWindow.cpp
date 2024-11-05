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

        MinimizeEnabled(false);
        MaximizeEnabled(false);

        Title("About Crystal Editor");
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

        FNativeContext* context = FNativeContext::Create(window, "AboutWindow", rootContext);

        AboutWindow* aboutWindow = nullptr;

        FAssignNewOwned(AboutWindow, aboutWindow, context);
        context->SetOwningWidget(aboutWindow);

        return aboutWindow;
    }
}

