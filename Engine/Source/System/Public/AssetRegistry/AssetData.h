#pragma once

namespace CE
{

	struct SYSTEM_API AssetData
	{
		// - Constructors -

		AssetData() {}

		// - Fields -

		// Path to the package that contains this asset. Ex: /Game/Textures/TextureAtlas02
		Name packageName{};

		// Path of the primary asset within the package. Ex: TextureAtlas.Noise04
		Name assetName{};

		/// Path to the primary asset's class. Ex: /Code/System.CE::Texture
		AssetPath assetClassPath;

		// Path to the source asset
		IO::Path sourceAssetPath{};
	};
    
} // namespace CE
