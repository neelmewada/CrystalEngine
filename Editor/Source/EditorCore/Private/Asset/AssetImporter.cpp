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

	bool AssetImporter::ImportSourceAssets(const Array<IO::Path>& sourceAssets, const Array<IO::Path>& productAssets)
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
			job->SetAutoDelete(false);

			job->Process();
			job->Finish();
			delete job;
		}
		jobs.Clear();

		return true;
	}

	Array<AssetImportJob*> AssetImporter::PrepareImportJobs(const Array<IO::Path>& sourceAssets, const Array<IO::Path>& productAssets)
	{
		if (productAssets.GetSize() != sourceAssets.GetSize())
		{
			CE_LOG(Error, All, "Failed to PrepareImportJobs(): productAssets array does not match the size of sourceAssets");
			return {};
		}

		auto jobs = CreateImportJobs(sourceAssets, productAssets);

		for (auto job : jobs)
		{
			job->includePaths = includePaths;
			job->targetPlatform = targetPlatform;
			job->SetAutoDelete(true);
		}

		return jobs;
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

		if (job->success)
		{
			AssetManager::GetRegistry()->OnAssetImported(job->bundleName);
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

		bundleName = "";
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
			bundleName = IO::Path::GetRelative(productPath, gProjectPath).RemoveExtension().GetString().Replace({ '\\' }, '/');
			if (!bundleName.StartsWith("/"))
				bundleName = "/" + bundleName;
		}
		else if (IO::Path::IsSubDirectory(productPath, engineRootDir / "Engine/Assets"))
		{
			isGameAsset = false;
			bundleName = IO::Path::GetRelative(productPath, engineRootDir).RemoveExtension().GetString().Replace({ '\\' }, '/');
			if (!bundleName.StartsWith("/"))
				bundleName = "/" + bundleName;
		}
		else
		{
			bundleName = assetName;
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
		
		Bundle* bundle = nullptr;

		if (productPath.Exists())
			bundle = Bundle::LoadBundleFromDisk(nullptr, productPath, LOAD_Full);
		else
			bundle = CreateObject<Bundle>(nullptr, bundleName);

		success = ProcessAsset(bundle);

		if (!success)
		{
			if (bundle)
				bundle->Destroy();
			return;
		}

		if (!isGameAsset)
		{
			// TODO: Non-game assets (i.e. Non-user assets) must have a fixed UUID based on their path.
			// Because They are generated locally when the engine is built.

			HashMap<Name, int> objectPathNameCounter{};

			std::function<void(Object*)> visitorFunc = [&](Object* object)
				{
					if (!object)
						return;

					if (object->IsOfType<Asset>())
					{
						Asset* asset = static_cast<Asset*>(object);
						asset->GetClass()->FindField("sourceAssetRelativePath")->SetFieldValue(asset, sourceAssetRelativePath);
					}

					String pathInBundle = object->GetPathInBundle().GetString();
					if (pathInBundle.NonEmpty())
						pathInBundle = "." + pathInBundle;
					Name fullObjectPath = bundle->GetBundleName().GetString() + pathInBundle;
					if (objectPathNameCounter[fullObjectPath] > 0)
					{
						fullObjectPath = String::Format(fullObjectPath.GetString() + "_{}", objectPathNameCounter[fullObjectPath]);
					}

					SIZE_T hash = GetHash(fullObjectPath);
					bundle->SetObjectUuid(object, Uuid(hash));

					objectPathNameCounter[fullObjectPath]++;

					for (int i = 0; i < object->GetSubObjectCount(); ++i)
					{
						Object* subObject = object->GetSubobject(i);
						visitorFunc(subObject);
					}
				};

			visitorFunc(bundle);
		}

		auto saveResult = Bundle::SaveBundleToDisk(bundle, nullptr, productPath);
		bundle->Destroy();

		if (saveResult != BundleSaveResult::Success)
		{
			errorMessage = "Failed to save bundle " + bundleName + " at path: " + productPath.GetString() + ". Error " + (int)saveResult;
			success = false;
			return;
		}
	}
    
} // namespace CE::Editor
