#pragma once

namespace CE
{

	/*
	* Asset Definition class. Gives information about different asset types.
	*/
    CLASS(Abstract)
    class SYSTEM_API AssetDefinition : public Object
    {
        CE_CLASS(AssetDefinition, Object)
    public:

		virtual ~AssetDefinition();

		virtual const Array<String>& GetSourceExtensions() = 0;

		virtual ClassType* GetAssetClass() = 0;

		virtual ClassType* GetAssetClass(const String& extension) = 0;

	private:

    };

	CLASS()
	class SYSTEM_API AssetDefinitionRegistry : public Object
	{
		CE_CLASS(AssetDefinitionRegistry, Object)
	public:

		AssetDefinitionRegistry();
		virtual ~AssetDefinitionRegistry();

		static AssetDefinitionRegistry* Get();

		AssetDefinition* FindAssetDefinitionForSourceAssetExtension(const String& extension);

	private:

		void OnClassRegistered(ClassType* classType);

		Array<AssetDefinition*> assetDefinitions{};

		DelegateHandle handle = 0;
	};
    
} // namespace CE

#include "AssetDefinition.rtti.h"
