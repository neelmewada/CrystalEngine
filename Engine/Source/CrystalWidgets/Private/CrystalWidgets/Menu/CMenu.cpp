#include "CrystalWidgets.h"

namespace CE::Widgets
{

    CMenu::CMenu()
    {
        canBeClosed = canBeMaximized = canBeMinimized = false;
    }

    CMenu::~CMenu()
    {
	    
    }

    void CMenu::Show()
    {
        if (nativeWindow)
        {
            nativeWindow->Show();
            return;
        }

        PlatformWindowInfo windowInfo{};
        windowInfo.windowFlags = PlatformWindowFlags::SkipTaskbar | PlatformWindowFlags::PopupMenu;
        windowInfo.fullscreen = windowInfo.maximised = windowInfo.resizable = false;
        windowInfo.hidden = true;

        nativeWindow = PlatformApplication::Get()->CreatePlatformWindow(title, showSize.width, showSize.height, windowInfo);
        nativeWindow->SetWindowPosition(showPosition);
        nativeWindow->SetBorderless(true);
        nativeWindow->SetAlwaysOnTop(true);
        nativeWindow->Show();

        RHI::SwapChainDescriptor desc{};
        desc.imageCount = CApplication::Get()->numFramesInFlight;
        desc.preferredFormats = { RHI::Format::R8G8B8A8_UNORM, RHI::Format::B8G8R8A8_UNORM };

        nativeWindow->GetWindowSize(&desc.preferredWidth, &desc.preferredHeight);

        if (swapChain)
        {
            RHI::gDynamicRHI->DestroySwapChain(swapChain);
        }

        swapChain = RHI::gDynamicRHI->CreateSwapChain(nativeWindow, desc);

        ConstructWindow();

        CApplication::Get()->windows.Add(this);
    }

    void CMenu::Show(Vec2i screenPosition, Vec2i size)
    {
        Super::Show(screenPosition, size);
    }

    void CMenu::Hide()
    {
        Super::Hide();
    }

    void CMenu::OnSubobjectAttached(Object* subobject)
    {
	    Super::OnSubobjectAttached(subobject);
        
        if (!subobject)
            return;

        if (subobject->IsOfType<CMenuItem>())
        {
            menuItems.Add(static_cast<CMenuItem*>(subobject));
        }
    }

    void CMenu::OnSubobjectDetached(Object* subobject)
    {
        Super::OnSubobjectDetached(subobject);

        if (!subobject)
            return;

        if (subobject->IsOfType<CMenuItem>())
        {
            menuItems.Remove(static_cast<CMenuItem*>(subobject));
        }
    }

} // namespace CE::Widgets
