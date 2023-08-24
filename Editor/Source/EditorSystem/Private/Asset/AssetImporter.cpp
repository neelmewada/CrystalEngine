#include "EditorSystem.h"

namespace CE::Editor
{
    
	AssetImporter::AssetImporter()
	{

	}

	AssetImporter::~AssetImporter()
	{

	}

	void AssetImporter::ImportSourceAssets(const Array<IO::Path>& sourcePaths, const Array<IO::Path>& productPaths)
	{
		auto jobs = CreateImportJobs(sourcePaths, productPaths);

		for (auto job : jobs)
		{
			numJobsInProgress++;
			job->Start();
		}
	}

	void AssetImporter::OnAssetImportJobFinish(bool success, const IO::Path& sourcePath, const Name& packageName)
	{
		mutex.Lock();
		if (success)
			numThumbnailsInProgress++;
		emit OnAssetImportResult(success, sourcePath, packageName);
		CE_LOG(Info, All, "Asset import {}: {}", success ? "successful" : "failed", sourcePath);
		numJobsInProgress--;
		mutex.Unlock();
	}

	void AssetImporter::OnAssetThumbnaiJobFinish(bool success, const Name& packageName)
	{

	}

	void AssetImportJob::Finish()
	{
		Super::Finish();

		importer->OnAssetImportJobFinish(success, sourcePath, outPackagePath);
	}


} // namespace CE::Editor
