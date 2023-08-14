#pragma once

namespace CE::Editor
{
	class EDITORSYSTEM_API AssetImportJob : public Job
	{
	public:
		typedef AssetImportJob Self;
		typedef Job Super;

		AssetImportJob(AssetImporter* importer, const IO::Path& sourcePath, const IO::Path& outPath) 
			: Job(true)
			, importer(importer)
			, sourcePath(sourcePath)
			, outPath(outPath)
		{

		}

		void Finish() override;

	protected:

		bool success = false;
		IO::Path sourcePath{};
		IO::Path outPath{};
		Name outPackagePath{};

		AssetImporter* importer = nullptr;
	};

    CLASS(Abstract)
    class EDITORSYSTEM_API AssetImporter : public Object
    {
        CE_CLASS(AssetImporter, Object)
    public:

		AssetImporter();
		virtual ~AssetImporter();

		void ImportSourceAssets(const Array<IO::Path>& sourcePaths, const Array<IO::Path>& productPaths);

		void OnAssetImportJobFinish(bool success, const IO::Path& sourcePath, const Name& packageName);

		/// Params: bool success, IO::Path sourcePath, Name outPackageName
		CE_SIGNAL(OnAssetImportResult, bool, IO::Path, Name);
        
    protected:

		virtual Array<AssetImportJob*> CreateImportJobs(const Array<IO::Path>& sourcePaths, const Array<IO::Path>& productPaths) = 0;
        
		Mutex mutex{};

	private:
        
    };

} // namespace CE::Editor

#include "AssetImporter.rtti.h"

