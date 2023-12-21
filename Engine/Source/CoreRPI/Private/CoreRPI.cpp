
#include "CoreRPI.h"

#include "Resource.h"
#include "CoreRPI.private.h"


namespace CE::RPI
{
    class CoreRPIModule : public CE::Module
    {
    public:
        virtual void StartupModule() override
        {
			classRegHandle = CoreObjectDelegates::onClassRegistered.AddDelegateInstance(
				MemberDelegate(&FeatureProcessorRegistry::OnClassRegistered, &FeatureProcessorRegistry::Get()));
			classDeregHandle = CoreObjectDelegates::onClassDeregistered.AddDelegateInstance(
				MemberDelegate(&FeatureProcessorRegistry::OnClassDeregistered, &FeatureProcessorRegistry::Get()));
        }

        virtual void ShutdownModule() override
        {
			FeatureProcessorRegistry::Get().featureProcessorClasses.Clear();

			CoreObjectDelegates::onClassRegistered.RemoveDelegateInstance(classRegHandle);
			CoreObjectDelegates::onClassDeregistered.RemoveDelegateInstance(classDeregHandle);
        }

        virtual void RegisterTypes() override
        {

        }

		DelegateHandle classRegHandle = 0;
		DelegateHandle classDeregHandle = 0;
    };
}

CE_IMPLEMENT_MODULE(CoreRPI, CE::CoreRPIModule)
