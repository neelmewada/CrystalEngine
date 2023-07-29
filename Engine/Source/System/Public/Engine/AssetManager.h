#pragma once

namespace CE
{

	CLASS()
	class SYSTEM_API AssetManager : public CE::Object
	{
		CE_CLASS(AssetManager, CE::Object)
	public:

		AssetManager();
		virtual ~AssetManager();

		static AssetManager* Get();

		static AssetRegistry* GetRegistry();

		virtual void Initialize();
		virtual void Shutdown();

		virtual void Tick(f32 deltaTime);

	protected:

		FIELD()
		AssetRegistry* assetRegistry = nullptr;

		friend class Engine;
	};
    
} // namespace CE

#include "AssetManager.rtti.h"
