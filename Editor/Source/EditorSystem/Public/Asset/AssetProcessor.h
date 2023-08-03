#pragma once

namespace CE::Editor
{
    CLASS(Abstract)
    class EDITORSYSTEM_API AssetProcessor : public Object
    {
        CE_CLASS(AssetProcessor, Object)
    public:

		AssetProcessor();
		virtual ~AssetProcessor();

		virtual Name ProcessSourceAsset(const IO::Path& sourcePath) = 0;
        
    private:
        
        
    };

} // namespace CE::Editor

#include "AssetProcessor.rtti.h"

