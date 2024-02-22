#include "EditorCore.h"

namespace CE::Editor
{

	const Array<String>& ShaderAssetDefinition::GetSourceExtensions()
	{
		static const Array<String> extensions = {
			".shader"
		};
		return extensions;
	}

	SubClassType<AssetImporter> ShaderAssetDefinition::GetAssetImporterClass()
	{
		return GetStaticClass<ShaderAssetImporter>();
	}
    
} // namespace CE::Editor
