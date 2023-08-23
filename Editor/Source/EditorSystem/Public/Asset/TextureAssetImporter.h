#pragma once

namespace CE::Editor
{
	class AssetImportJob;

    CLASS(Config = Engine)
	class EDITORSYSTEM_API TextureAssetImporter : public AssetImporter
	{
		CE_CLASS(TextureAssetImporter, AssetImporter)
	public:

		//Name ImportSourceAsset2(const IO::Path& sourceAssetPath, const IO::Path& productAssetPath = {}, bool linkSourceAsset = true);

		bool GenerateThumbnail(const Name& packagePath, BinaryBlob& outThumbnailPNG) override;

	protected:

		virtual Array<AssetImportJob*> CreateImportJobs(const Array<IO::Path>& sourcePaths, const Array<IO::Path>& productPaths) override;

		FIELD(Config)
		b8 compressByDefault = false;

		FIELD(Config)
		b8 highQualityCompression = false;

		FIELD(Config)
		f32 compressionQuality = 0.05f;
	};

	class EDITORSYSTEM_API TextureImportJob : public AssetImportJob
	{
	public:

		TextureImportJob(TextureAssetImporter* importer, const IO::Path& sourcePath, const IO::Path& outPath);

		void Process() override;

		b8 compressByDefault = false;
		b8 highQualityCompression = false;
		f32 compressionQuality = 0.05f;
	};

} // namespace CE::Editor

#include "TextureAssetImporter.rtti.h"
