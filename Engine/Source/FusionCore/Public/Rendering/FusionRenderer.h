#pragma once

namespace CE
{
    CLASS()
    class FUSIONCORE_API FusionRenderer : public Object
    {
        CE_CLASS(FusionRenderer, Object)
    public:

        FusionRenderer();

    protected:

        struct alignas(16) DrawItem2D
        {
	        
        };

    };
    
} // namespace CE

#include "FusionRenderer.rtti.h"
