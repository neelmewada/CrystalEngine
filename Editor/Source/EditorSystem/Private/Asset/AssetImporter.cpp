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
			job->Start();
		}
	}

	void AssetImporter::OnAssetImportJobFinish(bool success, const IO::Path& sourcePath, const Name& packageName)
	{
		mutex.Lock();
		CE_LOG(Info, All, "Asset import {}: {}", success ? "successful" : "failed", sourcePath);
		OnAssetImportResult(success, sourcePath, packageName);
		mutex.Unlock();
	}

	void AssetImportJob::Finish()
	{
		Super::Finish();

		importer->OnAssetImportJobFinish(success, sourcePath, outPackagePath);
	}

} // namespace CE::Editor
