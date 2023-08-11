
#include "System.h"


namespace CE
{
	JobManager* gJobManager = nullptr;
	JobContext* gJobContext = nullptr;

    class SystemModule : public Module
    {
    public:

        void StartupModule() override
        {
			JobManagerDesc desc{};
			desc.threads = {

			};
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

