#include "FusionCore.h"

namespace CE
{
    static FusionApplication* gInstance = nullptr;

    FusionApplication::FusionApplication()
    {

    }

    FusionApplication::~FusionApplication()
    {
        if (gInstance == this)
        {
            gInstance = nullptr;
        }
    }

    FusionApplication* FusionApplication::Get()
    {
        if (gInstance == nullptr)
        {
            gInstance = CreateObject<FusionApplication>(GetTransientPackage(MODULE_NAME), "FusionApplication");
        }
        return gInstance;
    }

    FusionApplication* FusionApplication::TryGet()
    {
        return gInstance;
    }

    void FusionApplication::Initialize(const FusionInitInfo& initInfo)
    {

    }

    void FusionApplication::Shutdown()
    {
    }

    void FusionApplication::Tick()
    {

    }

    void FusionApplication::InitializeShaders()
    {

    }

} // namespace CE

