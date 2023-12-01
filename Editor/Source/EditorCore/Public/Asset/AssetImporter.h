#pragma once

namespace CE::Editor
{
	class AssetImportJob;

    CLASS(Abstract, Config = Editor)
    class EDITORCORE_API AssetImporter : public Object
    {
        CE_CLASS(AssetImporter, Object)
    public:
        
        AssetImporter();
        virtual ~AssetImporter();

		void ImportSourceAssetsAsync(const Array<IO::Path>& sourceAssets);

		void ImportSourceAssetsAsync(const Array<IO::Path>& sourceAssets, const Array<IO::Path>& productAssets);
        
		inline bool IsImportInProgress() const { return numJobsInProgress > 0; }

		inline int GetNumJobsLeft() const { return numJobsInProgress; }

    protected:

		void OnAssetImportJobFinish(AssetImportJob* job);
        
		virtual Array<AssetImportJob*> CreateImportJobs(const Array<IO::Path>& sourceAssets, const Array<IO::Path>& productAssets) = 0;

		SharedMutex mutex{};
		int numJobsInProgress = 0;

		friend class AssetImportJob;
    };

	class EDITORCORE_API AssetImportJob : public Job
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

} // namespace CE::Editor

#include "AssetImporter.rtti.h"
