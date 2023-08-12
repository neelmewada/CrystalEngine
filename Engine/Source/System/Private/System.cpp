
#include "System.h"


namespace CE
{
	static JobManager* gJobManager = nullptr;
	static JobContext* gJobContext = nullptr;

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
    
    SYSTEM_API JobManager* GetJobManager()
    {
        return gJobManager;
    }

} // namespace CE


#include "System.private.h"
CE_IMPLEMENT_MODULE(System, CE::SystemModule)

