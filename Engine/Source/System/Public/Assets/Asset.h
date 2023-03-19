#pragma once

#include "CoreMinimal.h"

namespace CE
{
    enum class AssetType
    {
        None = 0,
        
        Scene,
        Shader,
        Texture,
    };

    class SYSTEM_API Asset : public Object
    {
        CE_CLASS(Asset, Object)
    protected:
        Asset(CE::Name name);
        virtual ~Asset();

    public:

        virtual AssetType GetAssetType() = 0;

    protected:
        IO::Path assetPath{};

    };

} // namespace CE

CE_RTTI_CLASS(SYSTEM_API, CE, Asset,
    CE_SUPER(CE::Object),
    CE_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST()
)
