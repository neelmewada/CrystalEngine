#pragma once

namespace CE
{
	class AssetManager;
	class GameInstance;
	class EngineSubsystem;

	CLASS(Abstract, NonSerialized, Config = Engine)
	class SYSTEM_API Engine : public Object
	{
		CE_CLASS(Engine, Object)
	public:
		// - Functions -

		virtual void PreInit();
		virtual void Initialize();

		virtual void PreShutdown();
		virtual void Shutdown();

		virtual void Tick(f32 deltaTime);

		void DispatchOnMainThread(Delegate<void(void)> action);

		virtual GameInstance* GetGameInstance();

		inline bool IsInitialized() const { return isInitialized; }

		inline AssetManager* GetAssetManager() const { return assetManager; }

		EngineSubsystem* GetSubsystem(ClassType* subsystemClass);

		template<typename TSubsystem> requires TIsBaseClassOf<EngineSubsystem, TSubsystem>::Value
		FORCE_INLINE TSubsystem* GetSubsystem()
		{
			return (TSubsystem*)GetSubsystem(TSubsystem::StaticType());
		}

	system_internal:

		// - Internal API -

		EngineSubsystem* CreateSubsystem(ClassType* subsystemClass);

	protected: 
		// - Fields -

		FIELD(Config)
		SubClassType<AssetManager> runtimeAssetManagerClass = nullptr;

		FIELD(Config)
		SubClassType<GameInstance> gameInstanceClass = nullptr;

		FIELD()
		AssetManager* assetManager = nullptr;

		FIELD()
		Array<GameInstance*> gameInstances = {};

		FIELD()
		Array<EngineSubsystem*> engineSubsystems{};

		Queue<Delegate<void()>> mainThreadQueue{};
		SharedMutex mainThreadQueueMutex{};

		b8 isInitialized = false;

	system_internal:
		static Array<ClassType*> subsystemClassQueue;

	};
    
} // namespace CE

#include "Engine.rtti.h"
