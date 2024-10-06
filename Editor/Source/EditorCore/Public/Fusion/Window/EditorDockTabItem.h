#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORCORE_API EditorDockTabItem : public FStyledWidget
    {
        CE_CLASS(EditorDockTabItem, FStyledWidget)
    public:

        // - Public API -

        bool SupportsMouseEvents() const override { return true; }

    protected:

        EditorDockTabItem();

        void Construct() override;

        void HandleEvent(FEvent* event) override;

        void OnPostComputeLayout() override;

        FLabel* label = nullptr;

        bool isActive = false;
        bool isHovered = false;

    public:

        FUSION_PROPERTY_WRAPPER(Text, label);

        FUSION_WIDGET;
        friend class EditorDockspaceStyle;
    };
    
}

#include "EditorDockTabItem.rtti.h"
