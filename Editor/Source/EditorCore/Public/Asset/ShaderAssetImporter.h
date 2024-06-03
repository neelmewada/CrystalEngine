#pragma once

namespace CE::Editor
{

	CLASS(Config = Editor)
	class EDITORCORE_API ShaderAssetImporter : public AssetImporter
	{
		CE_CLASS(ShaderAssetImporter, AssetImporter)
	public:

		virtual Array<AssetImportJob*> CreateImportJobs(const Array<IO::Path>& sourceAssets, const Array<IO::Path>& productAssets) override;

	private:


	};

	class EDITORCORE_API ShaderAssetImportJob : public AssetImportJob
	{
	public:
		typedef ShaderAssetImportJob Self;
		typedef AssetImportJob Super;

		ShaderAssetImportJob(AssetImporter* importer, const IO::Path& sourcePath, const IO::Path& outPath)
			: Super(importer, sourcePath, outPath)
		{
			
		}

		virtual bool ProcessAsset(Bundle* bundle) override;

	};
    
} // namespace CE::Editor

#include "ShaderAssetImporter.rtti.h"
