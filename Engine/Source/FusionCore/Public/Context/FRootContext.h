#pragma once

namespace CE
{
    CLASS()
    class FUSIONCORE_API FRootContext : public FFusionContext
    {
        CE_CLASS(FRootContext, FFusionContext)
    public:

        FRootContext();

        virtual ~FRootContext();

        void TickInput() override;

        void TickNativeContextInput(FNativeContext* nativeContext);

    private:


    };
    
} // namespace CE

#include "FRootContext.rtti.h"
