#include "FusionCore.h"

namespace CE
{

    FStyleSet::FStyleSet()
    {
        
    }

    void FStyleSet::Add(const Name& name, FStyle* style)
    {
        styles.Add(name, style);
    }

    void FStyleSet::Remove(const Name& styleName)
    {
        styles.Remove(styleName);
    }

    FStyle* FStyleSet::FindStyle(const Name& styleName)
    {
        if (styles.KeyExists(styleName) && styles[styleName] != nullptr)
        {
            return styles[styleName];
        }
        if (parent != nullptr)
        {
            return parent->FindStyle(styleName);
        }

        return nullptr;
    }

    void FStyleSet::SetParent(FStyleSet* parent)
    {
        this->parent = parent;
    }

} // namespace CE
