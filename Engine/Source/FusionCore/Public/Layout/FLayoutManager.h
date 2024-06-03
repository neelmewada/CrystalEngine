#pragma once

namespace CE
{
    class FFusionContext;

    CLASS()
    class FUSIONCORE_API FLayoutManager : public Object
    {
        CE_CLASS(FLayoutManager, Object)
    public:

        FLayoutManager();

    private:

        FIELD()
        FFusionContext* context = nullptr;

        FUSION_FRIENDS;
    };
    
} // namespace CE

#include "FLayoutManager.rtti.h"