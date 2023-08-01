#pragma once

namespace CE
{

	struct SYSTEM_API AssetData
	{
		// - Constructors -

		AssetData() {}

		// - Fields -

		// Path to the package that contains this asset. Ex: /Game/Textures/DoorTexture
		Name packageName{};

		// Path of the primary asset within the package. Ex: TextureMap
		Name assetName{};

		/// Path to the primary asset's class. Ex: /Code/System.CE::Texture
		Name assetClassPath;

		// Path to the source asset
		IO::Path sourceAssetPath{};
	};
    
} // namespace CE
