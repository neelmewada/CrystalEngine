
#include "CoreRPI.h"

#include "CoreRPI.private.h"


namespace CE
{
    class CoreRPIModule : public CE::Module
    {
    public:
        virtual void StartupModule() override
        {

        }

        virtual void ShutdownModule() override
        {

        }

        virtual void RegisterTypes() override
        {

        }
    };
}

CE_IMPLEMENT_MODULE(CoreRPI, CE::CoreRPIModule)
