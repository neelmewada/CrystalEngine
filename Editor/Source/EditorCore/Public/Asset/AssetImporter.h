#pragma once

namespace CE::Editor
{
    CLASS(Abstract)
    class EDITORCORE_API AssetImporter : public Object
    {
        CE_CLASS(AssetImporter, Object)
    public:
        
        AssetImporter();
        virtual ~AssetImporter();
        
    protected:
        
        
    };

} // namespace CE::Editor

#include "AssetImporter.rtti.h"
