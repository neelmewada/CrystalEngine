
#include "System.h"


namespace CE
{

    class SystemModule : public Module
    {
    public:

        void StartupModule() override
        {

        }

        void ShutdownModule() override
        {

        }

        void RegisterTypes() override
        {

        }

    };
    
} // namespace CE


#include "System.private.h"
CE_IMPLEMENT_MODULE(System, CE::SystemModule)

