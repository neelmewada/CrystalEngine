
#include <Asset/Asset.h>

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
        return Self::Type()->GetName();
    }

	void Asset::RegisterAssetClass(ClassType* assetClass, String assetExtensions)
	{
		if (!assetClass->CanBeInstantiated())
			return;

		assetExtensions = assetExtensions.RemoveWhitespaces();
		Array<String> list = assetExtensions.Split(',');

		for (auto extension : list)
		{
            if (!extension.StartsWith("."))
                extension = "." + extension;

			extensionToAssetClassMap[extension] = assetClass;
		}
	}

	bool Asset::IsValidAssetType(String assetExtension)
	{
        if (!assetExtension.StartsWith("."))
            assetExtension = "." + assetExtension;

		return extensionToAssetClassMap.KeyExists(assetExtension);
	}

	ClassType* Asset::GetAssetClassFor(String assetExtension)
	{
        if (!assetExtension.StartsWith("."))
            assetExtension = "." + assetExtension;

		return extensionToAssetClassMap[Name(assetExtension)];
	}

    BuiltinAssetType Asset::GetBuiltinAssetTypeFor(String assetExtension)
    {
        if (!assetExtension.StartsWith("."))
            assetExtension = "." + assetExtension;

        if (assetExtension == ".cscene")
            return BuiltinAssetType::SceneAsset;
        return BuiltinAssetType::None;
    }

    String Asset::GetAssetExtensionFor(BuiltinAssetType builtinAssetType)
    {
        switch (builtinAssetType)
        {
            case BuiltinAssetType::None:
                break;
            case BuiltinAssetType::SceneAsset:
                return ".cscene";
        }
        return "";
    }

} // namespace CE



