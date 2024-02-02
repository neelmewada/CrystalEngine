#include "CoreRPI.h"

namespace CE::RPI
{

    PassRegistry& PassRegistry::Get()
    {
		static PassRegistry instance{};
		return instance;
    }

	SubClass<Pass> PassRegistry::GetPassClass(const Name& shortName)
	{
		if (!registry.KeyExists(shortName))
			return nullptr;

		return registry[shortName].passClass;
	}

	void PassRegistry::OnClassRegistered(ClassType* classType)
	{
		if (classType != nullptr && classType->IsSubclassOf<Pass>())
		{
			Name shortName = classType->GetName().GetLastComponent();
			
			Entry& entry = registry[shortName];
			entry.shortName = shortName;
			entry.passClass = classType;
		}
	}

	void PassRegistry::OnClassDeregistered(ClassType* classType)
	{
		if (classType != nullptr && classType->IsSubclassOf<Pass>())
		{
			Name shortName = classType->GetName().GetLastComponent();

			registry.Remove(shortName);
		}
	}

} // namespace CE::RPI
