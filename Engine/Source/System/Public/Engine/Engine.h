#pragma once

namespace CE
{
	class AssetManager;
	class GameInstance;
	class EngineSubsystem;
	class GameViewport;

	CLASS(Abstract, NonSerialized, Config = Engine)
	class SYSTEM_API Engine : public Object
	{
		CE_CLASS(Engine, Object)
	public:
		// - Functions -

		virtual void PreInit();
		virtual void Initialize();
		virtual void PostInitialize();

		virtual void PreShutdown();
		virtual void Shutdown();

		virtual void Tick(f32 deltaTime);

		virtual void Render();

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

		void SetPrimaryGameViewport(GameViewport* gameViewport);

		inline GameViewport* GetPrimaryGameViewport() const { return primaryViewport; }

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

		CE::Shader* equirectShader = nullptr;

		CE::Queue<Delegate<void()>> mainThreadQueue{};
		SharedMutex mainThreadQueueMutex{};

		b8 isInitialized = false;

		GameViewport* primaryViewport = nullptr;

	system_internal:
		static Array<ClassType*> subsystemClassQueue;

	};
    
} // namespace CE

#include "Engine.rtti.h"
