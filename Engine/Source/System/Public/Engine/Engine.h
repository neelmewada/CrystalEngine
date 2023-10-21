#pragma once

namespace CE
{
	class AssetManager;

	CLASS(Abstract, NonSerialized)
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

	public: 
		// - Fields -

		FIELD()
		SubClassType<AssetManager> runtimeAssetManagerClass = nullptr;

		FIELD()
		AssetManager* assetManager = nullptr;

		Queue<Delegate<void()>> mainThreadQueue{};
		SharedMutex mainThreadQueueMutex{};
	};
    
} // namespace CE

#include "Engine.rtti.h"
