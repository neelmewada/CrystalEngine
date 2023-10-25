#pragma once

namespace CE::Editor
{
	class AssetImporterJob;

    CLASS(Abstract)
    class EDITORCORE_API AssetImporter : public Object
    {
        CE_CLASS(AssetImporter, Object)
    public:
        
        AssetImporter();
        virtual ~AssetImporter();

		void ImportSourceAssetsAsync(const Array<IO::Path>& sourceAssets, const Array<IO::Path>& productAssets);
        
		inline bool IsImportInProgress() const { return numJobsInProgress > 0; }

		inline int GetNumJobsLeft() const { return numJobsInProgress; }

    protected:
        
		virtual Array<AssetImporterJob*> CreateImportJobs(const Array<IO::Path>& sourceAssets, const Array<IO::Path>& productAssets) = 0;
        
		int numJobsInProgress = 0;
    };

} // namespace CE::Editor

#include "AssetImporter.rtti.h"
