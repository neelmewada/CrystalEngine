
#include "Asset/TextureAssetImportSettings.h"

namespace CE::Editor
{
	bool TextureAssetImportSettings::ApplyImportSettings(Asset* targetAsset)
	{
		if (targetAsset == nullptr || targetAsset->GetType()->GetTypeId() != TYPEID(TextureAsset))
			return false;

		TextureAsset* textureAsset = (TextureAsset*)targetAsset;
        
        textureAsset->textureType = textureType;

		return true;
	}
}

