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

        void HandleEvent(FEvent* event) override;

    public: // - Public API -

        bool IsSelected() const { return isSelected; }
        bool IsHovered() const { return isHovered; }

        bool SupportsMouseEvents() const override { return true; }

        bool CheckParentRowRecursive(ComponentTreeViewRow* checkRow);

    protected:

        ComponentTreeView* owner = nullptr;
        FHorizontalStack* content = nullptr;
        FImage* icon = nullptr;
        FLabel* label = nullptr;
        FImageButton* arrow = nullptr;
        bool isSelected = false;
        bool isHovered = false;
        ComponentTreeItem* item = nullptr;
        ComponentTreeViewRow* parentRow = nullptr;

    public: // - Fusion Properties -

        FUSION_PROPERTY_WRAPPER2(Background, icon, Icon);
        FUSION_PROPERTY_WRAPPER2(Text, label, Title);

        Self& Indentation(f32 value);
        Self& ArrowExpanded(bool value);
        Self& ArrowVisible(bool value);
        Self& IconVisible(bool value);

        FUSION_WIDGET;
        friend class ComponentTreeView;
        friend class ComponentTreeViewStyle;
    };
    
}

#include "ComponentTreeViewRow.rtti.h"
