#include "EditorCore.h"

namespace CE::Editor
{
    
	Array<AssetImportJob*> ShaderAssetImporter::CreateImportJobs(const Array<IO::Path>& sourceAssets, const Array<IO::Path>& productAssets)
	{
		Array<AssetImportJob*> jobs{};

		for (int i = 0; i < sourceAssets.GetSize(); i++)
		{
			jobs.Add(new ShaderAssetImportJob(this, sourceAssets[i], productAssets[i]));
		}

		return jobs;
	}


	bool ShaderAssetImportJob::ProcessAsset(Package* package)
	{
		if (package == nullptr)
			return false;

		if (!sourcePath.Exists())
			return false;
		
		CE::Shader* shader = package->LoadObject<CE::Shader>();

		if (shader == nullptr)
		{
			shader = CreateObject<CE::Shader>(package, TEXT("Shader"));
		}
		else
		{
			// Clean up old data
			if (shader->preprocessData != nullptr)
				shader->preprocessData->Destroy();
			for (auto pass : shader->passes)
			{
				pass->Destroy();
			}
			shader->passes.Clear();
		}

		shader->GetClass()->FindFieldWithName("sourceAssetRelativePath", TYPEID(String))->SetFieldValue(shader, sourceAssetRelativePath);

		FileStream fileReader = FileStream(sourcePath, Stream::Permissions::ReadOnly);
		fileReader.SetAsciiMode(true);

		ShaderPreprocessor preprocessor{ &fileReader, includePaths };

		shader->preprocessData = preprocessor.PreprocessShader(shader);

		if (shader->preprocessData == nullptr || shader->preprocessData->subShaders.IsEmpty())
		{
			errorMessage = preprocessor.GetErrorMessage();
			return false;
		}

		const auto& passEntries = shader->preprocessData->subShaders[0].passes;
		
		for (const SubShaderPassEntry& passEntry : passEntries)
		{
			ShaderCompiler compiler{};
			
			ShaderBuildConfig buildConfig{};
			buildConfig.entry = passEntry.vertexEntry.GetString();
			buildConfig.stage = RHI::ShaderStage::Vertex;
			buildConfig.includeSearchPaths = includePaths;
			buildConfig.debugName = shader->preprocessData->shaderName.GetString();

			ShaderPass* pass = CreateObject<ShaderPass>(shader, passEntry.passName.GetString());
			shader->passes.Add(pass);
			pass->variants.Add(CE::ShaderVariant());
			CE::ShaderVariant& variant = pass->variants[0];
			variant.variantHash = 0;
			variant.shaderStageBlobs.Add(ShaderBlob{});
			variant.shaderStageBlobs.Add(ShaderBlob{});

			// - Vertex -

			ShaderBlob& vertBlob = variant.shaderStageBlobs[0];
			vertBlob.format = ShaderBlobFormat::Spirv;
			vertBlob.shaderStage = RHI::ShaderStage::Vertex;

			Array<std::wstring> extraArgs{};

			ShaderCompiler::ErrorCode result = compiler.BuildSpirv(passEntry.source.GetDataPtr(), (u32)passEntry.source.GetDataSize(), buildConfig, vertBlob.byteCode, extraArgs);
			if (result != ShaderCompiler::ERR_Success)
			{
				errorMessage = "Failed to compile vertex shader. Error: " + compiler.GetErrorMessage();
				return false;
			}

			ShaderReflector shaderReflector{};
			ShaderReflector::ErrorCode reflectionResult = 
				shaderReflector.Reflect(ShaderBlobFormat::Spirv, vertBlob.byteCode.GetDataPtr(), vertBlob.byteCode.GetDataSize(), 
					RHI::ShaderStage::Vertex, variant.reflectionInfo);
			if (reflectionResult != ShaderReflector::ERR_Success)
			{
				errorMessage = "Failed to reflect vertex shader. Error: " + compiler.GetErrorMessage();
				return false;
			}

			// - Fragment -

			buildConfig.entry = passEntry.fragmentEntry.GetString();
			buildConfig.stage = RHI::ShaderStage::Fragment;
			ShaderBlob& fragBlob = variant.shaderStageBlobs[1];
			fragBlob.format = ShaderBlobFormat::Spirv;
			fragBlob.shaderStage = RHI::ShaderStage::Fragment;

			result = compiler.BuildSpirv(passEntry.source.GetDataPtr(), (u32)passEntry.source.GetDataSize(), buildConfig, fragBlob.byteCode, extraArgs);
			if (result != ShaderCompiler::ERR_Success)
			{
				errorMessage = "Failed to compile fragment shader. Error: " + compiler.GetErrorMessage();
				return false;
			}

			reflectionResult = shaderReflector.Reflect(ShaderBlobFormat::Spirv, fragBlob.byteCode.GetDataPtr(), fragBlob.byteCode.GetDataSize(),
				RHI::ShaderStage::Fragment, variant.reflectionInfo);
			if (reflectionResult != ShaderReflector::ERR_Success)
			{
				errorMessage = "Failed to reflect fragment shader. Error: " + compiler.GetErrorMessage();
				return false;
			}
		}

		return true;
	}

} // namespace CE::Editor
