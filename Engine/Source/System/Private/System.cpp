
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
			CoreObjectDelegates::onClassRegistered.RemoveDelegateInstance(classRegisteredHandle);
			CoreObjectDelegates::onClassDeregistered.RemoveDelegateInstance(classDeregisteredHandle);
        }

        void RegisterTypes() override
        {
			classRegisteredHandle = CoreObjectDelegates::onClassRegistered.AddDelegateInstance(MemberDelegate(&SystemModule::OnClassRegistered, this));
			classDeregisteredHandle = CoreObjectDelegates::onClassDeregistered.AddDelegateInstance(MemberDelegate(&SystemModule::OnClassDeregistered, this));
        }

		void OnClassRegistered(ClassType* classType)
		{
			if (classType->GetOwnerModuleName() == MODULE_NAME)
			{
				
			}
		}

		void OnClassDeregistered(ClassType* classType)
		{

		}

		DelegateHandle classRegisteredHandle = 0;
		DelegateHandle classDeregisteredHandle = 0;
    };

	/*
	*	Globals
	*/

	SYSTEM_API Engine* gEngine = nullptr;
    
} // namespace CE


#include "System.private.h"
CE_IMPLEMENT_MODULE(System, CE::SystemModule)

