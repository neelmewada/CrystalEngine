#pragma once

namespace CE
{
    CLASS()
    class FUSIONCORE_API FStyleManager : public Object
    {
        CE_CLASS(FStyleManager, Object)
    public:

        FStyleManager();

        virtual ~FStyleManager();

        void SetParent(FStyleManager* parent)
        {
            if (parent != this)
				this->parent = parent;
        }

        FStyleManager* GetParent() const { return parent; }

        void RegisterStyle(const Name& styleKey, FStyle* value);

        FStyle* FindStyle(const Name& styleKey);

    protected:

        FIELD()
        FStyleManager* parent = nullptr;

        HashMap<Name, FStyle*> registeredStyles{};
    };
    
} // namespace CE

#include "FStyleManager.rtti.h"
