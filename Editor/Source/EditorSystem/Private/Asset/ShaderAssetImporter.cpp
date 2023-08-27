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
		bool isGameAsset = false;

		if (outPath.IsEmpty())
			outPath = sourcePath.ReplaceExtension(".casset");
		if (!sourcePath.Exists())
			return;

		String sourceAssetRelativePath = "";

		if (IO::Path::IsSubDirectory(sourcePath, gProjectPath / "Game/Assets"))
		{
			sourceAssetRelativePath = IO::Path::GetRelative(sourcePath, gProjectPath).GetString().Replace({ '\\' }, '/');
			if (!sourceAssetRelativePath.StartsWith("/"))
				sourceAssetRelativePath = "/" + sourceAssetRelativePath;
		}

		String packageName = "";

		String assetName = outPath.GetFilename().RemoveExtension().GetString();
		if (!IsValidObjectName(assetName))
		{
			assetName = FixObjectName(assetName);
			outPath = outPath.GetParentPath() / (assetName + ".casset");
		}

		if (IO::Path::IsSubDirectory(outPath, gProjectPath / "Game/Assets"))
		{
			isGameAsset = true;
			packageName = IO::Path::GetRelative(outPath, gProjectPath).RemoveExtension().GetString().Replace({ '\\' }, '/');
			if (!packageName.StartsWith("/"))
				packageName = "/" + packageName;
		}
		else if (IO::Path::IsSubDirectory(outPath, PlatformDirectories::GetAppRootDir() / "Engine/Assets"))
		{
			packageName = IO::Path::GetRelative(outPath, PlatformDirectories::GetAppRootDir()).RemoveExtension().GetString().Replace({ '\\' }, '/');
			if (!packageName.StartsWith("/"))
				packageName = "/" + packageName;
		}
		else
		{
			packageName = assetName;
		}

		ShaderCompiler compiler{};
		
		auto editorShadersDir = PlatformDirectories::GetAppRootDir() / "Editor/Shaders";
		auto engineShadersDir = PlatformDirectories::GetEngineRootDir() / "Engine/Shaders";
		auto gameShadersDir = gProjectPath / "Game/Shaders";

		ShaderBuildConfig buildConfig{};
		buildConfig.includeSearchPaths.Add(engineShadersDir);
		if (IO::Path::IsSubDirectory(sourcePath, editorShadersDir))
			buildConfig.includeSearchPaths.Add(editorShadersDir);
		buildConfig.includeSearchPaths.Add(gameShadersDir);

		Array<std::wstring> extraArgs = {};

		ShaderReflection vertReflection{};
		ShaderReflection fragReflection{};

		// Vertex Shader
		buildConfig.entry = "VertMain";
		buildConfig.stage = ShaderStage::Vertex;

		void* vertByteCode = nullptr;
		u32 vertByteSize = 0;

		auto result = compiler.Build(ShaderBlobFormat::Spirv, sourcePath, buildConfig, &vertByteCode, &vertByteSize, extraArgs);
		if (result != ShaderCompiler::ERR_Success)
		{
			return;
		}

		defer(
			Memory::Free(vertByteCode);
		);

		ShaderReflector reflector{};
		auto result2 = reflector.Reflect(ShaderBlobFormat::Spirv, vertByteCode, vertByteSize, vertReflection);
		if (result2 != ShaderReflector::ERR_Success)
		{
			return;
		}

		void* fragByteCode = nullptr;
		u32 fragByteSize = 0;

		// Fragment Shader
		buildConfig.entry = "FragMain";
		buildConfig.stage = ShaderStage::Fragment;

		auto fragResult = compiler.Build(ShaderBlobFormat::Spirv, sourcePath, buildConfig, &fragByteCode, &fragByteSize, extraArgs);
		if (fragResult != ShaderCompiler::ERR_Success)
		{
			return;
		}

		defer(
			Memory::Free(fragByteCode);
		);
			
		reflector = ShaderReflector();
		auto fragReflResult = reflector.Reflect(ShaderBlobFormat::Spirv, fragByteCode, fragByteSize, fragReflection);
		if (fragReflResult != ShaderReflector::ERR_Success)
		{
			return;
		}
		
		Package* shaderPackage = nullptr;
		Shader* shader = nullptr;

		if (outPath.Exists())
		{
			shaderPackage = Package::LoadPackage(nullptr, outPath, LOAD_Full);
			if (shaderPackage != nullptr && shaderPackage->GetSubObjectCount() > 0)
			{
				for (auto [uuid, subobject] : shaderPackage->GetSubObjectMap())
				{
					if (subobject != nullptr && subobject->IsOfType<Shader>())
					{
						shader = (Shader*)subobject;
						break;
					}
				}
			}
		}

		if (shaderPackage == nullptr)
			shaderPackage = CreateObject<Package>(nullptr, packageName);
		if (shader == nullptr)
			shader = CreateObject<Shader>(shaderPackage, assetName);
		else if (shader->GetName() != assetName)
			shader->SetName(assetName);

		shader->variants.Add(ShaderVariant());
		ShaderVariant& variant = shader->variants.Top();

		variant.vertexShader = ShaderBlob();
		variant.vertexShader.blob.LoadData(vertByteCode, vertByteSize);
		variant.vertexShader.shaderStage = ShaderStage::Vertex;
		variant.vertexShader.reflectionInfo = vertReflection;
		variant.vertexShader.format = ShaderBlobFormat::Spirv;

		variant.fragmentShader = ShaderBlob();
		variant.fragmentShader.blob.LoadData(fragByteCode, fragByteSize);
		variant.fragmentShader.shaderStage = ShaderStage::Fragment;
		variant.fragmentShader.reflectionInfo = fragReflection;
		variant.fragmentShader.format = ShaderBlobFormat::Spirv;

		auto saveResult = Package::SavePackage(shaderPackage, shader);
		if (saveResult != SavePackageResult::Success)
		{
			return;
		}

		// Set output state
		outPackagePath = packageName;
		success = true;
	}

} // namespace CE::Editor
