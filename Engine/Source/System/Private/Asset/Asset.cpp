
#include "System.h"

namespace CE
{
	HashMap<Name, ClassType*> Asset::extensionToAssetClassMap{};

	Asset::Asset(CE::Name name) : Object(name)
	{

	}

	Asset::~Asset()
	{

	}

    CE::Name Asset::GetAssetType()
    {
        return Type()->GetName();
    }

	void Asset::RegisterAssetClass(ClassType* assetClass, String assetExtensions)
	{
		if (!assetClass->CanBeInstantiated())
			return;

		assetExtensions = assetExtensions.RemoveWhitespaces();
		Array<String> list = assetExtensions.Split(',');

		for (const auto& extension : list)
		{
			extensionToAssetClassMap[extension] = assetClass;
		}
	}

	bool Asset::IsValidAssetType(String assetExtension)
	{
		return extensionToAssetClassMap.KeyExists(assetExtension);
	}

	ClassType* Asset::GetAssetClassFor(String assetExtension)
	{
		return extensionToAssetClassMap[Name(assetExtension)];
	}

} // namespace CE



