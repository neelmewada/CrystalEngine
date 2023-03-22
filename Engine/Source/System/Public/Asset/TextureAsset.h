#pragma once

#include "CoreMinimal.h"

#include "Asset.h"

namespace CE
{
#if PAL_TRAIT_BUILD_EDITOR
    namespace Editor
    {
        class TextureAssetImportSettings;
    }
#endif

    class SYSTEM_API TextureAsset : public Asset
    {
        CE_CLASS(TextureAsset, Asset)
    public:
        TextureAsset();
        virtual ~TextureAsset();


    protected:

#if PAL_TRAIT_BUILD_EDITOR
        friend class CE::Editor::TextureAssetImportSettings;
#endif
    };
    
} // namespace CE

CE_RTTI_CLASS(SYSTEM_API, CE, TextureAsset,
    CE_SUPER(CE::Asset),
    CE_NOT_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST()
)


