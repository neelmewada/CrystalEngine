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

        void SelectItem(ComponentTreeItem* item);

        void Expand(ComponentTreeViewRow* expandRow);
        void Collapse(ComponentTreeViewRow* collapseRow);

        void ToggleExpand(ComponentTreeViewRow* row);

    public: // - Fusion Properties - 


    protected:

        Actor* actor = nullptr;
        FVerticalStack* content = nullptr;
        Array<ComponentTreeViewRow*> rows;
        Array<ComponentTreeItem*> items;
        HashSet<ComponentTreeItem*> expandedItems;

        ComponentTreeItem* selectedItem = nullptr;

        FUSION_WIDGET;
        friend class ComponentTreeViewStyle;
    };
    
}

#include "ComponentTreeView.rtti.h"
