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

	protected:


	private:

		FIELD()
		AssetRegistry* assetRegistry = nullptr;

		static AssetManager* singleton;
	};
    
} // namespace CE

#include "AssetManager.rtti.h"
