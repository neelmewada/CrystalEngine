#pragma once

namespace CE::Editor
{

	/*
	* Asset Definition class. Gives information about different asset types.
	*/
    CLASS(Abstract)
    class EDITORSYSTEM_API AssetDefinition : public Object
    {
        CE_CLASS(AssetDefinition, Object)
    public:

		virtual ~AssetDefinition();

		virtual const Array<String>& GetSourceExtensions() = 0;

		virtual ClassType* GetAssetClass() = 0;

		virtual ClassType* GetAssetClass(const String& extension) = 0;

		virtual SubClassType<AssetImporter> GetAssetImporterClass() = 0;

	private:

    };

	CLASS()
	class EDITORSYSTEM_API AssetDefinitionRegistry : public Object
	{
		CE_CLASS(AssetDefinitionRegistry, Object)
	public:

		AssetDefinitionRegistry();
		virtual ~AssetDefinitionRegistry();

		static AssetDefinitionRegistry* Get();

		static AssetDefinition* GetAssetDefinition(SubClassType<AssetDefinition> subClass);

		AssetDefinition* FindAssetDefinitionForSourceAssetExtension(const String& extension);

	private:

		void OnClassRegistered(ClassType* classType);

		Array<AssetDefinition*> assetDefinitions{};

		DelegateHandle handle = 0;
	};

	template<typename T> requires TIsBaseClassOf<AssetDefinition, T>::Value
	static inline T* GetAssetDefinition()
	{
		if (AssetDefinitionRegistry::Get() == nullptr)
			return nullptr;
		return (T*)AssetDefinitionRegistry::Get()->GetAssetDefinition(T::Type());
	}
    
} // namespace CE::Editor

#include "AssetDefinition.rtti.h"
