#include "FusionCore.h"

namespace CE
{

    FWindow::FWindow()
    {
        
    }

    void FWindow::ShowWindow()
    {
        PlatformWindow* platformWindow = GetPlatformWindow();
        if (platformWindow)
        {
            platformWindow->Show();
        }
    }

    void FWindow::HideWindow()
    {
        PlatformWindow* platformWindow = GetPlatformWindow();
        if (platformWindow)
        {
            platformWindow->Hide();
        }
    }

    PlatformWindow* FWindow::GetPlatformWindow()
    {
        FFusionContext* context = GetContext();
        if (context == nullptr)
            return nullptr;
        if (!context->IsOfType<FNativeContext>())
            return nullptr;

        FNativeContext* nativeContext = static_cast<FNativeContext*>(context);
        return nativeContext->GetPlatformWindow();
    }


}

