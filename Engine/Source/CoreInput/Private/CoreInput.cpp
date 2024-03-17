
#include "CoreInput.h"

#include "CoreInput.private.h"

namespace CE
{
    class CoreInputModule : public CE::Module
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

CE_IMPLEMENT_MODULE(CoreInput, CE::CoreInputModule)
