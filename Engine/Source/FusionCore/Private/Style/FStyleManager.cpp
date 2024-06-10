#include "FusionCore.h"

namespace CE
{

    FStyleManager::FStyleManager()
    {

    }

    FStyleManager::~FStyleManager()
    {
        
    }

    void FStyleManager::RegisterStyle(const Name& styleKey, FStyle* value)
    {
        registeredStyles[styleKey] = value;
    }

    FStyle* FStyleManager::FindStyle(const Name& styleKey)
    {
        if (registeredStyles.KeyExists(styleKey))
            return registeredStyles[styleKey];

        if (parent)
        {
            return parent->FindStyle(styleKey);
        }

        return nullptr;
    }

} // namespace CE

