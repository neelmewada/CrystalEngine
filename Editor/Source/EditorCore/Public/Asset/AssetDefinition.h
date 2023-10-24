#pragma once

namespace CE::Editor
{
    class AssetImporter;

    /// Asset Definition base class. Each asset type must have an asset definition class.
    CLASS(Abstract)
	class EDITORCORE_API AssetDefinition : public Object
	{
		CE_CLASS(AssetDefinition, Object)
	public:
        
        AssetDefinition();
        virtual ~AssetDefinition();
        
        /// Return an array of extensions of source files that produce this asset type.
        virtual const Array<String>& GetSourceExtensions() = 0;
        
        virtual SubClassType<Asset> GetDefaultAssetClass() = 0;
        
        virtual SubClassType<Asset> GetAssetClass(const String& extension) = 0;
        
        virtual SubClassType<AssetImporter> GetAssetImporterClass() = 0;
        
    protected:
        
	};

} // namespace CE::Editor

#include "AssetDefinition.rtti.h"
