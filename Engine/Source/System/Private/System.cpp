
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

			onClassRegistered = CoreObjectDelegates::onClassRegistered.AddDelegateInstance(MemberDelegate(&SystemModule::OnClassRegistered, this));
        }

        void ShutdownModule() override
        {
			CoreObjectDelegates::onClassDeregistered.RemoveDelegateInstance(onClassRegistered);

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

		void OnClassRegistered(ClassType* type)
		{
			if (type == nullptr)
				return;

			if (gEngine != nullptr && !engineSubsystemClasses.IsEmpty())
			{
				for (auto classType : engineSubsystemClasses)
				{
					gEngine->CreateSubsystem(classType);
				}

				engineSubsystemClasses.Clear();
			}

			if (type->GetTypeId() != TYPEID(EngineSubsystem) && 
				type->IsSubclassOf<EngineSubsystem>())
			{
				if (gEngine == nullptr) // Engine has not been initialize yet, cache the subsystem class type.
				{
					engineSubsystemClasses.Add(type);
				}
				else
				{
					gEngine->CreateSubsystem(type);
				}
			}
		}

		Array<ClassType*> engineSubsystemClasses{};

		DelegateHandle onClassRegistered = 0;
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

