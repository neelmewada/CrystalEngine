
#include "System.h"

#include "Resource.h"

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
            JOB_THREAD_PRIMARY;
            
			gJobManager = new JobManager("JobSystemManager", desc);
			gJobContext = new JobContext(gJobManager);
			JobContext::PushGlobalContext(gJobContext);

			onClassRegistered = CoreObjectDelegates::onClassRegistered.AddDelegateInstance(MemberDelegate(&SystemModule::OnClassRegistered, this));
        }

        void ShutdownModule() override
        {
			CoreObjectDelegates::onClassRegistered.RemoveDelegateInstance(onClassRegistered);

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

			if (type->GetTypeId() != TYPEID(EngineSubsystem) && 
				type->IsSubclassOf<EngineSubsystem>())
			{
				if (gEngine == nullptr || !gEngine->IsInitialized()) // Engine has not been initialize yet, cache the subsystem class type.
				{
					Engine::subsystemClassQueue.Add(type);
				}
				else
				{
					gEngine->CreateSubsystem(type);
				}
			}

			if (type->GetTypeId() != TYPEID(GameInstanceSubsystem) &&
				type->IsSubclassOf<GameInstanceSubsystem>())
			{
				if (gEngine == nullptr || gEngine->GetGameInstance() == nullptr || !gEngine->GetGameInstance()->IsInitialized())
				{
					GameInstance::subsystemClassesQueue.Add(type);
				}
				else
				{
					gEngine->GetGameInstance()->CreateSubsystem(type);
				}
			}
		}

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

