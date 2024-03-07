#pragma once

namespace CE::Editor
{
    CLASS(Config = Editor)
    class EDITORCORE_API StaticMeshAssetImporter : public AssetImporter
    {
        CE_CLASS(StaticMeshAssetImporter, AssetImporter)
    public:
        
        virtual Array<AssetImportJob*> CreateImportJobs(const Array<IO::Path>& sourceAssets, const Array<IO::Path>& productAssets) override;

    private:


    };

	class EDITORCORE_API StaticMeshAssetImportJob : public AssetImportJob
	{
	public:
		typedef StaticMeshAssetImportJob Self;
		typedef AssetImportJob Super;

		StaticMeshAssetImportJob(AssetImporter* importer, const IO::Path& sourcePath, const IO::Path& outPath)
			: Super(importer, sourcePath, outPath)
		{

		}

		virtual bool ProcessAsset(Package* package) override;

	};
    
} // namespace CE::Editor

#include "StaticMeshAssetImporter.rtti.h"
