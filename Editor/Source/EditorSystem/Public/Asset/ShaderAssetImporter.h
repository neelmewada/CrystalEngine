#pragma once

namespace CE::Editor
{

	CLASS(Config = Editor)
	class EDITORSYSTEM_API ShaderAssetImporter : public AssetImporter
	{
		CE_CLASS(ShaderAssetImporter, AssetImporter)
	public:

		bool GenerateThumbnail(const Name& packagePath, BinaryBlob& outThumbnailPNG) override;

	protected:

		virtual Array<AssetImportJob*> CreateImportJobs(const Array<IO::Path>& sourcePaths, const Array<IO::Path>& productPaths) override;


	};

	class EDITORSYSTEM_API ShaderImportJob : public AssetImportJob
	{
	public:

		ShaderImportJob(ShaderAssetImporter* importer, const IO::Path& sourcePath, const IO::Path& outPath);

		void Process() override;


	};
    
} // namespace CE::Editor

#include "ShaderAssetImporter.rtti.h"
