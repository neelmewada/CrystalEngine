#pragma once

namespace CE
{
    class FStyleSet;

    CLASS()
    class FUSIONCORE_API FStyleManager : public Object
    {
        CE_CLASS(FStyleManager, Object)
    public:

        FStyleManager();

        virtual ~FStyleManager();

        void RegisterStyleSet(const Name& name, FStyleSet* styleSet);
        void DeregisterStyleSet(const Name& name);

    protected:

        HashMap<Name, FStyleSet*> registeredStyleSets;

    };
    
} // namespace CE

#include "FStyleManager.rtti.h"
