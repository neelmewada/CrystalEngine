#pragma once

namespace CE::Editor
{
    CLASS(Abstract)
    class EDITORSYSTEM_API AssetProcessor : public Object
    {
        CE_CLASS(AssetProcessor, Object)
    public:
        
        virtual SubClassType<Asset> GetAssetClass() = 0;
        
    private:
        
        
    };

} // namespace CE::Editor

#include "AssetProcessor.rtti.h"

