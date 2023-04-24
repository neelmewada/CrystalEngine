
#include <Asset/Asset.h>

#include "System.h"

namespace CE
{
	HashMap<Name, ClassType*> Asset::sourceExtensionToAssetClassMap{};
    HashMap<ClassType*, Name> Asset::assetClassToProductExtensionMap{};

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

	void Asset::RegisterAssetClass(ClassType* assetClass, String productAssetExtension, String assetExtensions)
	{
		if (!assetClass->CanBeInstantiated())
			return;

		assetExtensions = assetExtensions.RemoveWhitespaces();
		Array<String> list = assetExtensions.Split(',');

		for (auto extension : list)
		{
            if (!extension.StartsWith("."))
                extension = "." + extension;

            sourceExtensionToAssetClassMap[extension] = assetClass;
		}

        if (!productAssetExtension.StartsWith("."))
            productAssetExtension = "." + productAssetExtension;

        assetClassToProductExtensionMap[assetClass] = productAssetExtension;
	}

	bool Asset::IsValidAssetType(String assetExtension)
	{
        if (!assetExtension.StartsWith("."))
            assetExtension = "." + assetExtension;

		return sourceExtensionToAssetClassMap.KeyExists(assetExtension) || GetBuiltinAssetTypeFor(assetExtension) != BuiltinAssetType::None;
	}

	ClassType* Asset::GetAssetClassFor(String assetExtension)
	{
        if (!assetExtension.StartsWith("."))
            assetExtension = "." + assetExtension;
        if (!sourceExtensionToAssetClassMap.KeyExists(assetExtension))
            return nullptr;

		return sourceExtensionToAssetClassMap[Name(assetExtension)];
	}

    String Asset::GetProductAssetExtensionFor(ClassType* assetClass)
    {
        if (assetClass == nullptr || !assetClassToProductExtensionMap.KeyExists(assetClass))
            return "";

        return assetClassToProductExtensionMap[assetClass].GetString();
    }

    String Asset::GetProductAssetExtensionFor(String sourceAssetExtension)
    {
        if (!sourceAssetExtension.StartsWith("."))
            sourceAssetExtension = "." + sourceAssetExtension;

        auto assetClass = GetAssetClassFor(sourceAssetExtension);
        if (assetClass == nullptr)
            return "";

        return GetProductAssetExtensionFor(assetClass);
    }

    bool Asset::IsValidBuiltinAssetExtension(String builtinAssetExtension)
    {
        return GetBuiltinAssetTypeFor(builtinAssetExtension) != BuiltinAssetType::None;
    }

    bool Asset::IsValidProductAssetExtension(String productAssetExtension)
    {
        if (!productAssetExtension.StartsWith("."))
            productAssetExtension = "." + productAssetExtension;

        for (auto [assetClass, productExtension] : assetClassToProductExtensionMap)
        {
	        if (productAssetExtension == productExtension.GetString())
                return true;
        }

        return false;
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



