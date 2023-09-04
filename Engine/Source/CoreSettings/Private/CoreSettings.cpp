
#include "CoreSettings.h"


namespace CE
{
    class CoreSettingsModule : public CE::Module
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

#include "CoreSettings.private.h"

CE_IMPLEMENT_MODULE(CoreSettings, CE::CoreSettingsModule)
