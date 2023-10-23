#pragma once

namespace CE
{
	class GameInstanceSubsystem;

	CLASS()
	class SYSTEM_API GameInstance : public Object
	{
		CE_CLASS(GameInstance, Object)
	public:

		GameInstance();

		// - Public API -

		inline bool IsInitialized() const { return isInitailized; }

		GameInstanceSubsystem* CreateSubsystem(ClassType* subsystemClass);

		GameInstanceSubsystem* GetSubsystem(ClassType* subsystemClass);

		virtual void Initialize();
		virtual void Shutdown();

	protected:

		FIELD()
		Array<GameInstanceSubsystem*> subsystems{};

		bool isInitailized = false;

	system_internal:

		static Array<ClassType*> subsystemClassesQueue;

	};
    
} // namespace CE

#include "GameInstance.rtti.h"