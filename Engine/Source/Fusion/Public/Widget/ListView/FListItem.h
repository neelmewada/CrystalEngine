#pragma once

namespace CE
{
    CLASS()
    class FUSION_API FListItem : public Object
    {
        CE_CLASS(FListItem, Object)
    public:

        FListItem();

        virtual ~FListItem();

        FIELD()
        String title = "";

    };
    
} // namespace CE

#include "FListItem.rtti.h"
