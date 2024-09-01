#pragma once

namespace CE
{
    CLASS()
    class FUSIONCORE_API FStyleSet : public Object
    {
        CE_CLASS(FStyleSet, Object)
    public:

        FStyleSet();

        void Add(const Name& styleName, FStyle* style);

        void Add(FStyle* style);

        void Remove(const Name& styleName);

        FStyle* FindStyle(const Name& styleName);

        FStyleSet* GetParent() const { return parent; }

        void SetParent(FStyleSet* parent);

    protected:

        FIELD()
        FStyleSet* parent = nullptr;

        HashMap<Name, FStyle*> styles{};

    };
    
} // namespace CE

#include "FStyleSet.rtti.h"