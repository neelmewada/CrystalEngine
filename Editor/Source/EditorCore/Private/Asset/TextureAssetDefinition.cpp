#include "EditorCore.h"

namespace CE::Editor
{

	const Array<String>& TextureAssetDefinition::GetSourceExtensions()
	{
		static const Array<String> extensions = {
			".png", ".jpg", ".jpeg", ".hdr"
		};
		return extensions;
	}

	SubClassType<AssetImporter> TextureAssetDefinition::GetAssetImporterClass()
	{
		return GetStaticClass<TextureAssetImporter>();
	}

} // namespace CE::Editor
