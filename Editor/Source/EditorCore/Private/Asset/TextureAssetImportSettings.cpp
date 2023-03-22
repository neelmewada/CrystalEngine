
#include "Asset/TextureAssetImportSettings.h"

namespace CE::Editor
{
	bool TextureAssetImportSettings::ApplyImportSettings(Asset* targetAsset)
	{
		if (targetAsset == nullptr || targetAsset->GetType()->GetTypeId() != TYPEID(TextureAsset))
			return false;

		return false;
	}
}

CE_RTTI_CLASS_IMPL(EDITORCORE_API, CE::Editor, TextureAssetImportSettings)
