#pragma once

namespace CE::Editor
{
    CLASS()
	class EDITORSYSTEM_API TextureAssetImporter : public AssetImporter
	{
		CE_CLASS(TextureAssetImporter, AssetImporter)
	public:

		Name ImportSourceAsset(const IO::Path& sourceAssetPath, const IO::Path& productAssetPath = {}, bool linkSourceAsset = true) override;

	protected:

		FIELD()
		TextureType textureType = TextureType::Default;
	};

} // namespace CE::Editor

#include "TextureAssetImporter.rtti.h"
