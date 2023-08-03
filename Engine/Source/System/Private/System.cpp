
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

	/*
	*	Globals
	*/

	SYSTEM_API Engine* gEngine = nullptr;
    
} // namespace CE


#include "System.private.h"
CE_IMPLEMENT_MODULE(System, CE::SystemModule)

