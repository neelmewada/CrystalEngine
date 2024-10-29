#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORCORE_API ComponentTreeView : public FStyledWidget
    {
        CE_CLASS(ComponentTreeView, FStyledWidget)
    protected:

        ComponentTreeView();

        void Construct() override;

    public: // - Public API -

        void SetActor(Actor* actor);

    public: // - Fusion Properties - 


    protected:

        FVerticalStack* content = nullptr;
        Array<ComponentTreeItem*> items;
        HashSet<ComponentTreeItem*> expandedItems;

        FUSION_WIDGET;
    };
    
}

#include "ComponentTreeView.rtti.h"
