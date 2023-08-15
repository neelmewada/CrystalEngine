#pragma once

namespace CE::Editor
{
	class AssetImportJob;

    CLASS()
	class EDITORSYSTEM_API TextureAssetImporter : public AssetImporter
	{
		CE_CLASS(TextureAssetImporter, AssetImporter)
	public:

		//Name ImportSourceAsset2(const IO::Path& sourceAssetPath, const IO::Path& productAssetPath = {}, bool linkSourceAsset = true);

	protected:

		virtual Array<AssetImportJob*> CreateImportJobs(const Array<IO::Path>& sourcePaths, const Array<IO::Path>& productPaths) override;

	};

	class EDITORSYSTEM_API TextureImportJob : public AssetImportJob
	{
	public:

		TextureImportJob(TextureAssetImporter* importer, const IO::Path& sourcePath, const IO::Path& outPath);

		void Process() override;

	};

} // namespace CE::Editor

#include "TextureAssetImporter.rtti.h"
