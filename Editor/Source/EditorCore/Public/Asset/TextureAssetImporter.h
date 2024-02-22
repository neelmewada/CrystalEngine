#pragma once

namespace CE::Editor
{
    
    CLASS(Config = Editor)
	class EDITORCORE_API TextureAssetImporter : public AssetImporter
	{
		CE_CLASS(TextureAssetImporter, AssetImporter)
	public:

		virtual Array<AssetImportJob*> CreateImportJobs(const Array<IO::Path>& sourceAssets, const Array<IO::Path>& productAssets) override;

	private:

		FIELD()
		TextureCompressionQuality defaultQompressionQuality = TextureCompressionQuality::Default;
	};

	class EDITORCORE_API TextureAssetImportJob : public AssetImportJob
	{
	public:
		typedef TextureAssetImportJob Self;
		typedef AssetImportJob Super;

		TextureAssetImportJob(AssetImporter* importer, const IO::Path& sourcePath, const IO::Path& outPath)
			: Super(importer, sourcePath, outPath)
		{

		}

		virtual bool ProcessAsset(Package* package) override;

	private:

		TextureCompressionQuality compressionQuality = TextureCompressionQuality::Default;

		friend class TextureAssetImporter;
	};

} // namespace CE::Editor

#include "TextureAssetImporter.rtti.h"