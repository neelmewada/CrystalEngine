
#include "Sandbox.h"

#include "Sandbox.private.h"


namespace CE
{
    class SandboxModule : public CE::Module
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

CE_IMPLEMENT_MODULE(Sandbox, CE::SandboxModule)
