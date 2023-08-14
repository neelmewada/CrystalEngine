#pragma once

namespace CE::Editor
{
    
	CLASS()
	class EDITORSYSTEM_API TextureAssetDefinition : public AssetDefinition
	{
		CE_CLASS(TextureAssetDefinition, AssetDefinition)
	public:

		TextureAssetDefinition();
		virtual ~TextureAssetDefinition();

		const Array<String>& GetSourceExtensions() override;

		ClassType* GetAssetClass() override;

		ClassType* GetAssetClass(const String& extension) override;

		SubClassType<AssetImporter> GetAssetImporterClass() override;

		TextureSourceCompressionFormat GetSourceCompressionFormatFromExtension(const String& sourceExtension);

	protected:


	};

} // namespace CE::Editor

#include "TextureAssetDefinition.rtti.h"
