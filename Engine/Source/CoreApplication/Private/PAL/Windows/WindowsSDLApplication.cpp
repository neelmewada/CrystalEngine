
#include "CoreApplication.h"


namespace CE
{
    WindowsSDLApplication* WindowsSDLApplication::Create()
    {
        return new WindowsSDLApplication();
    }

    void WindowsSDLApplication::Initialize()
    {
        Super::Initialize();

    }

    WindowsSDLApplication::WindowsSDLApplication()
    {

    }

}
