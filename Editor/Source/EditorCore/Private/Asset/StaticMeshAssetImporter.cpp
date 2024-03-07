#include "EditorCore.h"

namespace CE::Editor
{

    Array<AssetImportJob*> StaticMeshAssetImporter::CreateImportJobs(const Array<IO::Path>& sourceAssets, const Array<IO::Path>& productAssets)
    {
        Array<AssetImportJob*> jobs{};

		for (int i = 0; i < sourceAssets.GetSize(); i++)
		{
			auto job = new StaticMeshAssetImportJob(this, sourceAssets[i], productAssets[i]);
			
            

			jobs.Add(job);
		}

        return jobs;
    }

    bool StaticMeshAssetImportJob::ProcessAsset(Package* package)
    {
		if (package == nullptr)
			return false;
		if (!sourcePath.Exists())
			return false;

		// Clear the package of any subobjects/assets, we will build the asset from scratch
		package->DestroyAllSubobjects();

		String extension = sourcePath.GetFilename().GetExtension().GetString();
		String fileName = sourcePath.GetFilename().RemoveExtension().GetString();
		// Make sure we can use the fileName as name of an object
		fileName = FixObjectName(fileName);

		FileStream fileStream = FileStream(sourcePath, CE::Stream::Permissions::ReadOnly);
		fileStream.SetBinaryMode(true);
		u8* data = (u8*)malloc(fileStream.GetLength());
		fileStream.Read(data, fileStream.GetLength());

		ModelImporter importer{};
		ModelLoadConfig config{};
		config.postProcessFlags |= ModelPostProcessFlags::OptimizeMeshes | ModelPostProcessFlags::OptimizeGraph | ModelPostProcessFlags::GenNormals |
			ModelPostProcessFlags::CalcTangentSpace;
		config.fileFormat = ModelFileFormat::FBX;

		CMScene* scene = importer.ImportScene(data, fileStream.GetLength(), config);
		if (scene == nullptr)
		{
			errorMessage = importer.GetErrorMessage();
			return false;
		}

		defer(
			delete scene;
		);

		StaticMesh* staticMesh = CreateObject<StaticMesh>(package, fileName);

		

        return true;
    }

} // namespace CE::Editor
