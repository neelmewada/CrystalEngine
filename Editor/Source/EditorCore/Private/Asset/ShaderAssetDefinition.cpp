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

	SubClassType<Asset> ShaderAssetDefinition::GetDefaultAssetClass()
	{
		return GetStaticClass<CE::Shader>();
	}

	SubClassType<Asset> ShaderAssetDefinition::GetAssetClass(const String& extension)
	{
		return GetStaticClass<CE::Shader>();
	}

	SubClassType<AssetImporter> ShaderAssetDefinition::GetAssetImporterClass()
	{
		return GetStaticClass<ShaderAssetImporter>();
	}
    
} // namespace CE::Editor
