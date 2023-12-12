#include "CoreRPI.h"

namespace CE::RPI
{

    FeatureProcessorRegistry& FeatureProcessorRegistry::Get()
    {
		static FeatureProcessorRegistry instance{};
		return instance;
    }

	void FeatureProcessorRegistry::OnClassRegistered(ClassType* classType)
	{
		if (classType != nullptr && classType->IsSubclassOf<FeatureProcessor>() && classType->CanBeInstantiated())
		{
			featureProcessorClasses.Add(classType);
		}
	}

	void FeatureProcessorRegistry::OnClassDeregistered(ClassType* classType)
	{
		if (classType != nullptr && classType->IsSubclassOf<FeatureProcessor>() && classType->CanBeInstantiated())
		{
			featureProcessorClasses.Remove(classType);
		}
	}

} // namespace CE::RPI
