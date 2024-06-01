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

		// NOTE: The package might already have assets & objects stored in it if asset already existed
		// It is responsibility of the derived asset importer to clear the old objects or just modify them as per need.
		
		// Clear the package of any subobjects, we will build the asset from scratch
		package->DestroyAllSubobjects();

		Array<IO::Path> includePaths = this->includePaths;
		includePaths.Add(sourcePath.GetParentPath());

		CE::Shader* shader = package->LoadObject<CE::Shader>();

		if (shader == nullptr) // Create new object from scratch
		{
			shader = CreateObject<CE::Shader>(package, TEXT("Shader"));
		}

		shader->GetClass()->FindField("sourceAssetRelativePath", TYPEID(String))->SetFieldValue(shader, sourceAssetRelativePath);

		FileStream fileReader = FileStream(sourcePath, Stream::Permissions::ReadOnly);
		fileReader.SetAsciiMode(true);

		ShaderPreprocessor preprocessor{ &fileReader, includePaths };

		ShaderPreprocessData preprocessData{};
		
		preprocessor.PreprocessShader(&preprocessData);

		if (preprocessor.GetErrorMessage().NonEmpty())
		{
			errorMessage = preprocessor.GetErrorMessage();
			shader->Destroy();
			return false;
		}

		shader->shaderName = preprocessData.shaderName;

		shader->properties.Clear();
		for (int i = 0; i < preprocessData.properties.GetSize(); i++)
		{
			shader->properties.Add(preprocessData.properties[i]);
		}

		for (SubShaderEntry& subShaderEntry : preprocessData.subShaders)
		{
			auto& passEntries = subShaderEntry.passes;

			shader->subShaders.Add({});
			SubShader& subShader = shader->subShaders.Top();
			subShader.tags.AddRange(subShaderEntry.subShaderTags);
			int passIndex = 0;

			for (SubShaderPassEntry& passEntry : passEntries)
			{
				ShaderCompiler compiler{};

				ShaderBuildConfig buildConfig{};
				buildConfig.entry = passEntry.vertexEntry.GetString();
				buildConfig.stage = RHI::ShaderStage::Vertex;
				buildConfig.includeSearchPaths = includePaths;
				buildConfig.debugName = preprocessData.shaderName.GetString();

				subShader.passes.Add({});
				ShaderPass& pass = subShader.passes.Top();
				pass.passName = passEntry.passName.GetString();
				pass.variants.Add(CE::ShaderVariant());
				pass.tags.AddRange(passEntry.passTags);

				CE::ShaderVariant& variant = pass.variants[0];
				variant.variantHash = 0;
				variant.shaderStageBlobs.Add(CreateObject<ShaderBlob>(shader, String("VertexBlob_") + passIndex));
				variant.shaderStageBlobs.Add(CreateObject<ShaderBlob>(shader, String("FragmentBlob_") + passIndex));

				// - Vertex -

				ShaderBlob* vertBlob = variant.shaderStageBlobs[0];
				vertBlob->format = ShaderBlobFormat::Spirv;
				vertBlob->shaderStage = RHI::ShaderStage::Vertex;

				Array<std::wstring> vertexExtraArgs{};
				vertexExtraArgs.AddRange({
					L"-D", L"COMPILE=1",
					L"-D", L"VERTEX=1",
					L"-fspv-preserve-bindings",
					});

				ShaderCompiler::ErrorCode result = compiler.BuildSpirv(passEntry.source.GetDataPtr(), (u32)passEntry.source.GetDataSize(), buildConfig, vertBlob->byteCode, vertexExtraArgs);
				if (result != ShaderCompiler::ERR_Success)
				{
					errorMessage = "Failed to compile vertex shader. Error: " + compiler.GetErrorMessage();
					return false;
				}

				ShaderReflector shaderReflector{};
				ShaderReflector::ErrorCode reflectionResult =
					shaderReflector.Reflect(ShaderBlobFormat::Spirv, vertBlob->byteCode.GetDataPtr(), vertBlob->byteCode.GetDataSize(),
						RHI::ShaderStage::Vertex, variant.reflectionInfo);
				if (reflectionResult != ShaderReflector::ERR_Success)
				{
					errorMessage = "Failed to reflect vertex shader. Error: " + compiler.GetErrorMessage();
					return false;
				}

				// - Fragment -

				buildConfig.entry = passEntry.fragmentEntry.GetString();
				buildConfig.stage = RHI::ShaderStage::Fragment;
				ShaderBlob* fragBlob = variant.shaderStageBlobs[1];
				fragBlob->format = ShaderBlobFormat::Spirv;
				fragBlob->shaderStage = RHI::ShaderStage::Fragment;

				Array<std::wstring> fragmentExtraArgs{};
				fragmentExtraArgs.AddRange({
					L"-D", L"COMPILE=1",
					L"-D", L"FRAGMENT=1",
					L"-fspv-preserve-bindings",
					});

				result = compiler.BuildSpirv(passEntry.source.GetDataPtr(), (u32)passEntry.source.GetDataSize(), buildConfig, fragBlob->byteCode, fragmentExtraArgs);
				if (result != ShaderCompiler::ERR_Success)
				{
					errorMessage = "Failed to compile fragment shader. Error: " + compiler.GetErrorMessage();
					return false;
				}

				reflectionResult = shaderReflector.Reflect(ShaderBlobFormat::Spirv, fragBlob->byteCode.GetDataPtr(), fragBlob->byteCode.GetDataSize(),
					RHI::ShaderStage::Fragment, variant.reflectionInfo);
				if (reflectionResult != ShaderReflector::ERR_Success)
				{
					errorMessage = "Failed to reflect fragment shader. Error: " + compiler.GetErrorMessage();
					return false;
				}

				// Clear the original HLSL source
				passEntry.source.Free();
				passIndex++;
			}
		}
		
		return true;
	}

} // namespace CE::Editor
