#include "EditorCore.h"

namespace CE::Editor
{

    AssetImporter::AssetImporter()
    {
        
    }

    AssetImporter::~AssetImporter()
    {
        
    }

	void AssetImporter::ImportSourceAssetsAsync(const Array<IO::Path>& sourceAssets)
	{
		Array<IO::Path> productAssets{};

		for (const auto& sourceAsset : sourceAssets)
		{
			productAssets.Add(sourceAsset.ReplaceExtension(".casset"));
		}

		ImportSourceAssetsAsync(sourceAssets, productAssets);
	}

	void AssetImporter::ImportSourceAssetsAsync(const Array<IO::Path>& sourceAssets, const Array<IO::Path>& productAssets)
	{
		
	}

	void AssetImporter::OnAssetImportJobFinish(AssetImportJob* job)
	{
		LockGuard<SharedMutex> lock{ mutex };

		numJobsInProgress--;
	}

	void AssetImportJob::Finish()
	{
		importer->OnAssetImportJobFinish(this);
	}
    
} // namespace CE::Editor
