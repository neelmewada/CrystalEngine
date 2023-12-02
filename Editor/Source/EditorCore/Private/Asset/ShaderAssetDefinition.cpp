#include "EditorCore.h"

namespace CE::Editor
{

	const Array<String>& ShaderAssetDefinition::GetSourceExtensions()
	{
		static Array<String> extensions = {
			".shader"
		};
		return extensions;
	}

	SubClassType<Asset> ShaderAssetDefinition::GetDefaultAssetClass()
	{
		return GetStaticClass<Shader>();
	}

	SubClassType<Asset> ShaderAssetDefinition::GetAssetClass(const String& extension)
	{
		return GetStaticClass<Shader>();
	}

	SubClassType<AssetImporter> ShaderAssetDefinition::GetAssetImporterClass()
	{
		return GetStaticClass<ShaderAssetImporter>();
	}
    
} // namespace CE::Editor
