#pragma once

#include "CoreMinimal.h"

namespace CE
{

    struct SYSTEM_API ConfigBase
    {
        CE_STRUCT(ConfigBase)

    public:

        bool isInvalid = false;

    };
    
} // namespace CE

CE_RTTI_STRUCT(SYSTEM_API, CE, ConfigBase,
    CE_SUPER(),
    CE_ATTRIBS(),
    CE_FIELD_LIST(
        CE_FIELD(isInvalid, NonSerialized)
    )
)
