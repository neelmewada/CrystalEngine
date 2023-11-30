#pragma once

namespace CE::Editor
{

	CLASS()
	class EDITORCORE_API ShaderAssetImporter : public AssetImporter
	{
		CE_CLASS(ShaderAssetImporter, AssetImporter)
	public:

		virtual Array<AssetImportJob*> CreateImportJobs(const Array<IO::Path>& sourceAssets, const Array<IO::Path>& productAssets) override;

	};

	class EDITORCORE_API ShaderAssetImportJob : public AssetImportJob
	{
	public:



	};
    
} // namespace CE::Editor

#include "ShaderAssetImporter.rtti.h"
