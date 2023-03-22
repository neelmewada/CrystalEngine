#pragma once

#include "CoreMinimal.h"

namespace CE
{

    class SYSTEM_API Asset : public Object
    {
        CE_CLASS(Asset, Object)
    protected:
        Asset(CE::Name name);
        virtual ~Asset();
        
    public:


    protected:
        String assetName{};
        String assetExtension{};

    };

} // namespace CE

CE_RTTI_CLASS(SYSTEM_API, CE, Asset,
    CE_SUPER(CE::Object),
    CE_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(assetName, Hidden)
        CE_FIELD(assetExtension, Hidden)
    ),
    CE_FUNCTION_LIST()
)
