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

	}

	void AssetImporter::OnAssetImportJobFinish(bool success, const IO::Path& sourcePath, const Name& packageName)
	{
		mutex.Lock();
		OnAssetImportResult(success, sourcePath, packageName);
		mutex.Unlock();
	}

	void AssetImportJob::Finish()
	{
		Super::Finish();

		importer->OnAssetImportJobFinish(success, sourcePath, outPackagePath);
	}

} // namespace CE::Editor
