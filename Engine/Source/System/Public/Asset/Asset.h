#pragma once

#include "CoreMinimal.h"

namespace CE
{
#if PAL_TRAIT_BUILD_EDITOR
    namespace Editor
    {
        class AssetProcessor;
    }
#endif

    CLASS(Abstract)
    class SYSTEM_API Asset : public CE::Object
    {
        CE_CLASS(Asset, CE::Object)
    protected:
        Asset(CE::Name name);
        virtual ~Asset();
        
    public:
        
        Name GetAssetType();

    protected:
        FIELD(Hidden)
        String assetName{};

        FIELD(Hidden)
        String assetExtension{};
        
#if PAL_TRAIT_BUILD_EDITOR
        friend class CE::Editor::AssetProcessor;
#endif
    };

} // namespace CE

#include "Asset.rtti.h"

