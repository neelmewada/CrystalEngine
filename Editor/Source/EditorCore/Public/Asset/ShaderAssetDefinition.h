#pragma once

namespace CE::Editor
{

	CLASS()
	class EDITORCORE_API ShaderAssetDefinition : public AssetDefinition
	{
		CE_CLASS(ShaderAssetDefinition, AssetDefinition)
	public:

		/// Return an array of extensions of source files that produce this asset type.
		virtual const Array<String>& GetSourceExtensions() override;

		virtual SubClassType<Asset> GetDefaultAssetClass() override;

		virtual SubClassType<Asset> GetAssetClass(const String& extension) override;

		virtual SubClassType<AssetImporter> GetAssetImporterClass() override;

	};
    
} // namespace CE::Editor

#include "ShaderAssetDefinition.rtti.h"
