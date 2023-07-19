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

	public: 
		// - Fields -

		FIELD()
		AssetManager* assetManager = nullptr;
	};
    
} // namespace CE

#include "Engine.rtti.h"
