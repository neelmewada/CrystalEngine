#pragma once

namespace CE::Editor
{
    class ComponentTreeView;

    CLASS()
    class EDITORCORE_API ComponentTreeViewRow : public FStyledWidget
    {
        CE_CLASS(ComponentTreeViewRow, FStyledWidget)
    protected:

        ComponentTreeViewRow();

        void Construct() override;

    public: // - Public API -


    protected:

        FHorizontalStack* content = nullptr;
        FImage* icon = nullptr;
        FLabel* label = nullptr;

    public: // - Fusion Properties -

        FUSION_PROPERTY_WRAPPER2(Background, icon, Icon);
        FUSION_PROPERTY_WRAPPER2(Text, label, Title);

        FUSION_WIDGET;
        friend class ComponentTreeView;
    };
    
}

#include "ComponentTreeViewRow.rtti.h"
