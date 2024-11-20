
#include "CoreApplication.h"


namespace CE
{
    MacSDLApplication* MacSDLApplication::Create()
    {
        return new MacSDLApplication();
    }

    void MacSDLApplication::Initialize()
    {
        Super::Initialize();

    }

    MacSDLApplication::MacSDLApplication()
    {

    }

}
