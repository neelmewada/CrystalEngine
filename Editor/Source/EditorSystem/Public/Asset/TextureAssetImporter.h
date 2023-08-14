#pragma once

namespace CE::Editor
{
	class TextureAssetImporter;

	class EDITORSYSTEM_API TextureImportJob : public AssetImportJob
	{
	public:
		TextureImportJob(TextureAssetImporter* importer, const IO::Path& sourcePath, const IO::Path& outPath)
			: AssetImportJob(importer, sourcePath, outPath)
		{

		}

		void Process() override;

	};

    CLASS()
	class EDITORSYSTEM_API TextureAssetImporter : public AssetImporter
	{
		CE_CLASS(TextureAssetImporter, AssetImporter)
	public:

		Name ImportSourceAsset2(const IO::Path& sourceAssetPath, const IO::Path& productAssetPath = {}, bool linkSourceAsset = true);

	protected:

		virtual Array<AssetImportJob*> CreateImportJobs(const Array<IO::Path>& sourcePaths, const Array<IO::Path>& productPaths) override;

		FIELD()
		TextureType textureType = TextureType::Default;
	};

} // namespace CE::Editor

#include "TextureAssetImporter.rtti.h"
