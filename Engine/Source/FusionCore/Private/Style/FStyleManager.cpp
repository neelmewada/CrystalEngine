#include "FusionCore.h"

namespace CE
{

    FStyleManager::FStyleManager()
    {

    }

    FStyleManager::~FStyleManager()
    {
        
    }

    void FStyleManager::RegisterStyleSet(const Name& name, FStyleSet* styleSet)
    {
        registeredStyleSets.Add(name, styleSet);
    }

    void FStyleManager::DeregisterStyleSet(const Name& name)
    {
        FStyleSet* styleSet = registeredStyleSets[name];

        if (styleSet)
        {
            FFusionContext* rootContext = FusionApplication::Get()->rootContext;
            rootContext->OnStyleSetDeregistered(styleSet);
        }

        registeredStyleSets.Remove(name);
    }

} // namespace CE

