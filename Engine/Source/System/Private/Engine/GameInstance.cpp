#include "System.h"

namespace CE
{
	Array<ClassType*> GameInstance::subsystemClassesQueue{};

    GameInstance::GameInstance()
    {

    }

	GameInstanceSubsystem* GameInstance::CreateSubsystem(ClassType* subsystemClass)
	{
		if (subsystemClass == nullptr || 
			subsystemClass->GetTypeId() == TYPEID(GameInstanceSubsystem) ||
			!subsystemClass->IsSubclassOf<GameInstanceSubsystem>())
			return nullptr;

		auto cdi = (GameInstanceSubsystem*)subsystemClass->GetDefaultInstance();
		if (cdi == nullptr || !cdi->ShouldBeCreated(this))
			return nullptr;

		// Check if subsystem already exists. Only 1 subsystem object allowed per class type.
		int index = subsystems.IndexOf([&](GameInstanceSubsystem* subsystem) -> bool
			{ return subsystem != nullptr && subsystem->GetType()->GetTypeId() == subsystemClass->GetTypeId(); });

		if (index >= 0)
			return subsystems[index];

		String name = subsystemClass->GetName().GetLastComponent();
		GameInstanceSubsystem* subsystem = CreateObject<GameInstanceSubsystem>(this, name, OF_NoFlags, subsystemClass);
		if (subsystem != nullptr)
		{
			subsystems.Add(subsystem);
			if (isInitailized)
				subsystem->Initialize();
		}
	}

	void GameInstance::Initialize()
	{
		for (auto classType : subsystemClassesQueue)
		{
			CreateSubsystem(classType);
		}
		subsystemClassesQueue.Clear();

		for (auto subsystem : subsystems)
		{
			subsystem->Initialize();
		}

		isInitailized = true;
	}

	void GameInstance::Shutdown()
	{
		isInitailized = false;

		for (auto subsystem : subsystems)
		{
			subsystem->Shutdown();
		}
		for (auto subsystem : subsystems)
		{
			subsystem->Destroy();
		}
		subsystems.Clear();
	}

} // namespace CE

