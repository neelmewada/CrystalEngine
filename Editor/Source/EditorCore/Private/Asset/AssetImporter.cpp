#include "EditorCore.h"

namespace CE::Editor
{

    AssetImporter::AssetImporter()
    {
		targetPlatform = PlatformMisc::GetCurrentPlatform();
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

	bool AssetImporter::ImportSourceAssetsAsync(const Array<IO::Path>& sourceAssets, const Array<IO::Path>& productAssets)
	{
		if (productAssets.GetSize() != sourceAssets.GetSize())
		{
			CE_LOG(Error, All, "Failed to ImportSourceAssetsAsync(): productAssets array does not match the size of sourceAssets");
			return false;
		}

		auto jobs = CreateImportJobs(sourceAssets, productAssets);

		for (auto job : jobs)
		{
			job->includePaths = includePaths;
			job->targetPlatform = targetPlatform;
			job->SetAutoDelete(true);
			
			job->Start();
		}

		return true;
	}

	void AssetImporter::OnAssetImportJobFinish(AssetImportJob* job)
	{
		LockGuard<SharedMutex> lock{ mutex };

		AssetImportJobResult importResult{};
		importResult.success = job->success;
		importResult.sourcePath = job->sourcePath;
		importResult.productPath = job->productPath;
		importResult.errorMessage = job->errorMessage;

		if (!job->success)
		{
			CE_LOG(Error, All, "Failed to import asset {}. Error: {}", job->sourcePath, job->errorMessage);
		}
		else if (enableLogging)
		{
			CE_LOG(Info, All, "Generated Asset: {}", job->productPath);
		}

		importResults.Add(importResult);

		numJobsInProgress--;
	}

	void AssetImportJob::Finish()
	{
		importer->OnAssetImportJobFinish(this);
	}

	void AssetImportJob::Process()
	{
		success = false;
		errorMessage = "";

		if (!sourcePath.Exists() || sourcePath.IsDirectory())
		{
			errorMessage = "Source asset does not exist at path: " + sourcePath.GetString();
			return;
		}

		if (productPath.IsEmpty())
			productPath = sourcePath.ReplaceExtension(".casset");
		editorProductPath = sourcePath.ReplaceExtension(".casset");

		packageName = "";
		sourceAssetRelativePath = "";
		generateDistributionAsset = false;

		if (IO::Path::IsSubDirectory(sourcePath, gProjectPath / "Game/Assets"))
		{
			sourceAssetRelativePath = IO::Path::GetRelative(sourcePath, gProjectPath).GetString().Replace({ '\\' }, '/');
			if (!sourceAssetRelativePath.StartsWith("/"))
				sourceAssetRelativePath = "/" + sourceAssetRelativePath;
		}

		String assetName = productPath.GetFilename().RemoveExtension().GetString();
		if (!IsValidObjectName(assetName)) // Fix asset name
		{
			assetName = FixObjectName(assetName);
			productPath = productPath.GetParentPath() / (assetName + ".casset");
			editorProductPath = sourcePath.GetParentPath() / (assetName + ".casset");
		}

		auto engineRootDir = gProjectPath;
		if (!(engineRootDir / "Engine/Assets").Exists())
			engineRootDir = PlatformDirectories::GetAppRootDir();

		if (IO::Path::IsSubDirectory(productPath, gProjectPath / "Game/Assets"))
		{
			isGameAsset = true;
			packageName = IO::Path::GetRelative(productPath, gProjectPath).RemoveExtension().GetString().Replace({ '\\' }, '/');
			if (!packageName.StartsWith("/"))
				packageName = "/" + packageName;
		}
		else if (IO::Path::IsSubDirectory(productPath, engineRootDir / "Engine/Assets"))
		{
			isGameAsset = false;
			packageName = IO::Path::GetRelative(productPath, engineRootDir).RemoveExtension().GetString().Replace({ '\\' }, '/');
			if (!packageName.StartsWith("/"))
				packageName = "/" + packageName;
		}
		else
		{
			packageName = assetName;
		}
		
		if (editorProductPath != productPath)
		{
			// We are generating an asset that is NOT dependent on the source asset, ex: An asset to be used by standalone dist build.
			sourceAssetRelativePath = "";
			generateDistributionAsset = true;
		}

#if PLATFORM_WINDOWS
		// Replace backward slashes with forward ones
		sourcePath = sourcePath.GetString().Replace({ '\\' }, '/');
		editorProductPath = editorProductPath.GetString().Replace({ '\\' }, '/');
		productPath = productPath.GetString().Replace({'\\'}, '/');
#endif
		
		Package* package = nullptr;

		if (productPath.Exists())
			package = Package::LoadPackage(nullptr, productPath, LOAD_Full);
		else
			package = CreateObject<Package>(nullptr, packageName);

		success = ProcessAsset(package);

		if (!success)
		{
			if (package)
				package->Destroy();
			return;
		}

		auto saveResult = Package::SavePackage(package, nullptr, productPath);
		package->Destroy();

		if (saveResult != SavePackageResult::Success)
		{
			errorMessage = "Failed to save package " + packageName + " at path: " + productPath.GetString() + ". Error " + (int)saveResult;
			success = false;
			return;
		}
	}
    
} // namespace CE::Editor
