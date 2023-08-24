#pragma once

namespace CE
{

	struct SYSTEM_API AssetData
	{
		// - Constructors -

		AssetData() {}

		// - Fields -

		// Path to the package that contains this asset. Ex: /Game/Assets/Textures/DoorTexture
		Name packageName{};

		// UUID of the package that contains this asset.
		UUID packageUuid{};

		// Path of the primary asset within the package. Ex: TextureMap
		Name assetName{};

		// UUID of the primary asset within the package.
		UUID assetUuid{};

		/// Path to the primary asset's class. Ex: /Code/System.CE::Texture2D
		Name assetClassPath;

		// Path to the source asset. For Editor only!
		Name sourceAssetPath{};
	};

	template<>
	inline SIZE_T GetHash<AssetData>(const AssetData& assetData)
	{
		return GetCombinedHashes({ assetData.packageName.GetHashValue(), assetData.assetName.GetHashValue(), assetData.assetClassPath.GetHashValue() });
	}
    
} // namespace CE
