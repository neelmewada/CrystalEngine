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
        if (styleSet && name.IsValid())
        {
	        registeredStyleSets.Add(name, styleSet);
        }
    }

    void FStyleManager::RegisterStyleSet(FStyleSet* styleSet)
    {
        if (styleSet)
        {
	        RegisterStyleSet(styleSet->GetName(), styleSet);
        }
    }

    void FStyleManager::DeregisterStyleSet(const Name& name)
    {
        FStyleSet* styleSet = registeredStyleSets[name];

        if (styleSet)
        {
            FFusionContext* rootContext = FusionApplication::Get()->rootContext.Get();
            rootContext->OnStyleSetDeregistered(styleSet);
        }

        registeredStyleSets.Remove(name);
    }

} // namespace CE

