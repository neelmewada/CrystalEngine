#pragma once

namespace CE::Editor
{

    CLASS(Config = Editor)
    class EDITORCORE_API FontAssetImporter : public AssetImporter
    {
        CE_CLASS(FontAssetImporter, AssetImporter)
    public:
        
        virtual Array<AssetImportJob*> CreateImportJobs(const Array<IO::Path>& sourceAssets, const Array<IO::Path>& productAssets) override;

		Array<Name> GetProductAssetDependencies() override;

    private:

		
    };

	class EDITORCORE_API FontAssetImportJob : public AssetImportJob
	{
	public:
		typedef FontAssetImportJob Self;
		typedef AssetImportJob Super;

		FontAssetImportJob(AssetImporter* importer, const IO::Path& sourcePath, const IO::Path& outPath)
			: Super(importer, sourcePath, outPath)
		{

		}

		virtual bool ProcessAsset(Package* package) override;

	private:

		CMFontAtlasGenerateInfo genInfo{};

		friend class FontAssetImporter;
	};
    
} // namespace CE::Editor

#include "FontAssetImporter.rtti.h"