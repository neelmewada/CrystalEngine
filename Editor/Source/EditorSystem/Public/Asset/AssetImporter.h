#pragma once

namespace CE::Editor
{
    CLASS(Abstract)
    class EDITORSYSTEM_API AssetImporter : public Object
    {
        CE_CLASS(AssetImporter, Object)
    public:

		AssetImporter();
		virtual ~AssetImporter();

		virtual Name ImportSourceAsset(const IO::Path& sourceAssetPath, const IO::Path& productAssetPath = {}, bool linkSourceAsset = true) = 0;
        
    protected:
        
	private:
        
    };

} // namespace CE::Editor

#include "AssetImporter.rtti.h"

