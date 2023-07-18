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


	public: 
		// - Fields -

		FIELD()
		AssetManager* assetManager = nullptr;
	};
    
} // namespace CE

#include "Engine.rtti.h"
