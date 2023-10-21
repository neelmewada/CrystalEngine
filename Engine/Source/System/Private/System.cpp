
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
			desc.defaultTag = JOB_THREAD_WORKER;
			desc.totalThreads = 0; // auto set optimal number of threads
			desc.threads = { };

			gJobManager = new JobManager("JobSystemManager", desc);
			gJobContext = new JobContext(gJobManager);
			JobContext::PushGlobalContext(gJobContext);
        }

        void ShutdownModule() override
        {
			gJobManager->DeactivateWorkersAndWait();

			JobContext::PopGlobalContext();
			delete gJobContext;
			gJobContext = nullptr;
			delete gJobManager;
			gJobManager = nullptr;
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

