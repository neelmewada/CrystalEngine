#include "EditorCore.h"

namespace CE::Editor
{
	const Array<String>& FontAssetDefinition::GetSourceExtensions()
	{
		static const Array<String> extensions{
			".ttf"
		};

		return extensions;
	}

	SubClass<AssetImporter> FontAssetDefinition::GetAssetImporterClass()
	{
		return GetStaticClass<FontAssetImporter>();
	}

} // namespace CE::Editor
