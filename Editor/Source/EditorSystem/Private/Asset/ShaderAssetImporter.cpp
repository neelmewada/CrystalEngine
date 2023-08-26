#include "EditorSystem.h"

namespace CE::Editor
{
	bool ShaderAssetImporter::GenerateThumbnail(const Name& packagePath, BinaryBlob& outThumbnailPNG)
	{
		return false;
	}

	Array<AssetImportJob*> ShaderAssetImporter::CreateImportJobs(const Array<IO::Path>& sourcePaths, const Array<IO::Path>& productPaths)
	{
		Array<AssetImportJob*> result{};

		for (int i = 0; i < sourcePaths.GetSize(); i++)
		{
			IO::Path productPath = {};
			if (i < productPaths.GetSize())
				productPath = productPaths[i];

			auto job = new ShaderImportJob(this, sourcePaths[i], productPath);

			result.Add(job);
		}

		return result;
	}

	ShaderImportJob::ShaderImportJob(ShaderAssetImporter* importer, const IO::Path& sourcePath, const IO::Path& outPath)
		: AssetImportJob(importer, sourcePath, outPath)
	{
	}

	void ShaderImportJob::Process()
	{
		success = false;

		ShaderCompiler compiler{};
		
		ShaderBuildConfig buildConfig{};
		buildConfig.entry = "VertMain";
		buildConfig.stage = ShaderStage::Vertex;
		
		Array<std::wstring> extraArgs = {};

		void* byteCode = nullptr;
		u32 byteSize = 0;

		auto result = compiler.Build(ShaderBlobFormat::Spirv, sourcePath, buildConfig, &byteCode, &byteSize, extraArgs);
		if (result != ShaderCompiler::ERR_Success)
		{
			return;
		}
		

	}

} // namespace CE::Editor
