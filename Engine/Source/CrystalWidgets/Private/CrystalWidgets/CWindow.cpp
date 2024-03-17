#include "CrystalWidgets.h"

namespace CE::Widgets
{
    
    CWindow::CWindow()
    {
        windowResizeDelegate = 
            PlatformApplication::Get()->onWindowDrawableSizeChanged.AddDelegateInstance(
                MemberDelegate(&Self::OnWindowSizeChanged, this));
    }

    CWindow::~CWindow()
    {
        PlatformApplication::Get()->onWindowDrawableSizeChanged.RemoveDelegateInstance(windowResizeDelegate);
    }

    void CWindow::OnWindowSizeChanged(PlatformWindow* window, u32 newWidth, u32 newHeight)
    {
        if (this->nativeWindow == window)
        {
            // TODO: Window resize event
        }
    }

} // namespace CE::Widgets
