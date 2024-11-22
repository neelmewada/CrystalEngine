

#include "CoreApplication.h"

#include <Windows.h>
#include <shellscalingapi.h>

#pragma comment(lib, "shcore.lib")

namespace CE
{
    WindowsSDLApplication* WindowsSDLApplication::Create()
    {
        return new WindowsSDLApplication();
    }

    void WindowsSDLApplication::Initialize()
    {
        SetProcessDPIAware();

        Super::Initialize();
    }

    u32 WindowsSDLApplication::GetSystemDpi()
    {
        return GetDpiForSystem();
    }

    WindowsSDLApplication::WindowsSDLApplication()
    {

    }

}
