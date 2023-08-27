#include "EditorSystem.h"

namespace CE::Editor
{
    
	ShaderAssetDefinition::ShaderAssetDefinition()
	{
	}

	ShaderAssetDefinition::~ShaderAssetDefinition()
	{
	}

	const Array<String>& ShaderAssetDefinition::GetSourceExtensions()
	{
		static Array<String> extensions = {
			".hlsl"
		};
		return extensions;
	}

	ClassType* ShaderAssetDefinition::GetAssetClass()
	{
		return GetStaticClass<Shader>();
	}

	ClassType* ShaderAssetDefinition::GetAssetClass(const String& extension)
	{
		return GetStaticClass<Shader>();
	}

	SubClassType<AssetImporter> ShaderAssetDefinition::GetAssetImporterClass()
	{
		return GetStaticClass<ShaderAssetImporter>();
	}

} // namespace CE::Editor
